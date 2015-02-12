/*
Copyright (c) 2014 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "minko/render/DrawCallPool.hpp"

#include "minko/data/ResolvedBinding.hpp"

using namespace minko;
using namespace minko::render;

DrawCallPool::DrawCallPool() :
    _drawCalls(),
    _macroToDrawCalls(),
    _invalidDrawCalls(),
    _macroChangedSlot(),
    _propChangedSlot(),
    _zSortUsefulPropertyChangedSlot()
{
    _zSortUsefulPropertyNames = {
        "modelToWorldMatrix",
        "material[${materialUuid}].priority",
        "material[${materialUuid}].zSorted",
        "geometry[${geometryUuid}].position"
    };
}

DrawCallPool::DrawCallIteratorPair
DrawCallPool::addDrawCalls(Effect::Ptr                                          effect,
                           const std::string&                                   techniqueName,
                           const std::unordered_map<std::string, std::string>&  variables,
                           data::Store&                                         rootData,
                           data::Store&                                         rendererData,
                           data::Store&                                         targetData)
{
    const auto& technique = effect->technique(techniqueName);

    for (const auto& pass : technique)
    {
        DrawCall* drawCall = new DrawCall(pass, variables, rootData, rendererData, targetData);

        _drawCalls.push_back(drawCall);
        initializeDrawCall(*drawCall);
    }

    return DrawCallIteratorPair(std::prev(_drawCalls.end(), technique.size()), std::prev(_drawCalls.end()));
}

void
DrawCallPool::removeDrawCalls(const DrawCallIteratorPair& iterators)
{
    auto end = std::next(iterators.second);

    for (auto it = iterators.first; it != end; ++it)
    {
        DrawCall& drawCall = **it;

        unwatchProgramSignature(
            drawCall,
            drawCall.pass()->macroBindings(),
            drawCall.rootData(),
            drawCall.rendererData(),
            drawCall.targetData()
        );
        unbindDrawCall(drawCall);

        _invalidDrawCalls.erase(&drawCall);
    }

    _drawCalls.erase(iterators.first, end);
}

void
DrawCallPool::watchProgramSignature(DrawCall&                       drawCall,
                                    const data::MacroBindingMap&    macroBindings,
                                    data::Store&                    rootData,
                                    data::Store&                    rendererData,
                                    data::Store&                    targetData)
{
    for (const auto& macroNameAndBinding : macroBindings.bindings)
    {
        const auto& macroName = macroNameAndBinding.first;
        const auto& macroBinding = macroNameAndBinding.second;
        auto& store = macroBinding.source == data::Binding::Source::ROOT ? rootData
            : macroBinding.source == data::Binding::Source::RENDERER ? rendererData
            : targetData;
        auto propertyName = Store::getActualPropertyName(drawCall.variables(), macroBinding.propertyName);
        auto bindingKey = MacroBindingKey(propertyName, &store);
        auto& drawCalls = _macroToDrawCalls[bindingKey];

        drawCalls.push_back(&drawCall);

        if (hasMacroCallback(bindingKey))
            continue;

        if (macroBindings.types.at(macroName) != data::MacroBindingMap::MacroType::UNSET)
        {
            addMacroCallback(
                bindingKey,
                store.propertyChanged(propertyName),
                std::bind(&DrawCallPool::macroPropertyChangedHandler, this, std::ref(macroBinding), std::ref(drawCalls))
            );
        }
        else
        {
            if (store.hasProperty(propertyName))
            {
                addMacroCallback(
                    bindingKey,
                    store.propertyRemoved(propertyName),
                    std::bind(
                        &DrawCallPool::macroPropertyRemovedHandler,
                        this,
                        std::ref(macroBinding),
                        propertyName,
                        std::ref(store),
                        std::ref(drawCalls)
                    )
                );
            }
            else
            {
                addMacroCallback(
                    bindingKey,
                    store.propertyAdded(propertyName),
                    std::bind(
                        &DrawCallPool::macroPropertyAddedHandler,
                        this,
                        std::ref(macroBinding),
                        propertyName,
                        std::ref(store),
                        std::ref(drawCalls)
                    )
                );
            }

        }
    }
}

void
DrawCallPool::addMacroCallback(const MacroBindingKey&   key,
                               PropertyChanged&         signal,
                               const PropertyCallback&  callback)
{
    if (_macroChangedSlot.count(key) == 0)
        _macroChangedSlot.emplace(key, ChangedSlot(signal.connect(callback, 10), 1));
    else
        _macroChangedSlot[key].second++;
}

void
DrawCallPool::removeMacroCallback(const MacroBindingKey& key)
{
    _macroChangedSlot[key].second--;

    if (_macroChangedSlot[key].second == 0)
        _macroChangedSlot.erase(key);
}

bool
DrawCallPool::hasMacroCallback(const MacroBindingKey& key)
{
    return _macroChangedSlot.count(key) != 0;
}

void
DrawCallPool::macroPropertyChangedHandler(const data::MacroBinding& macroBinding, const std::list<DrawCall*>& drawCalls)
{
    _invalidDrawCalls.insert(drawCalls.begin(), drawCalls.end());
}

void
DrawCallPool::macroPropertyAddedHandler(const data::MacroBinding&   macroBinding,
                                        const std::string&          propertyName,
                                        data::Store&                store,
                                        const std::list<DrawCall*>& drawCalls)
{
    MacroBindingKey key(propertyName, &store);

    removeMacroCallback(key);
    addMacroCallback(
        key,
        store.propertyRemoved(),
        std::bind(
            &DrawCallPool::macroPropertyRemovedHandler,
            this,
            std::ref(macroBinding),
            propertyName,
            std::ref(store),
            std::ref(drawCalls)
        )
    );

    macroPropertyChangedHandler(macroBinding, drawCalls);
}

void
DrawCallPool::macroPropertyRemovedHandler(const data::MacroBinding&     macroBinding,
                                          const std::string&            propertyName,
                                          data::Store&                  store,
                                          const std::list<DrawCall*>&   drawCalls)
{
    MacroBindingKey key(propertyName, &store);

    removeMacroCallback(key);
    addMacroCallback(
        key,
        store.propertyAdded(),
        std::bind(
            &DrawCallPool::macroPropertyAddedHandler,
            this,
            std::ref(macroBinding),
            propertyName,
            std::ref(store),
            std::ref(drawCalls)
        )
    );

    macroPropertyChangedHandler(macroBinding, drawCalls);
}

void
DrawCallPool::unwatchProgramSignature(DrawCall&                     drawCall,
                                      const data::MacroBindingMap&  macroBindings,
                                      data::Store&                  rootData,
                                      data::Store&                  rendererData,
                                      data::Store&                  targetData)
{
    for (const auto& macroNameAndBinding : macroBindings.bindings)
    {
        const auto& macroBinding = macroNameAndBinding.second;
        auto& store = macroBinding.source == data::Binding::Source::ROOT ? rootData
            : macroBinding.source == data::Binding::Source::RENDERER ? rendererData
            : targetData;
        auto propertyName = Store::getActualPropertyName(drawCall.variables(), macroBinding.propertyName);
        auto bindingKey = MacroBindingKey(propertyName, &store);
        auto& drawCalls = _macroToDrawCalls.at(bindingKey);

        drawCalls.remove(&drawCall);

        if (drawCalls.size() == 0)
            _macroToDrawCalls.erase(bindingKey);

        removeMacroCallback(bindingKey);
    }
}

void
DrawCallPool::initializeDrawCall(DrawCall& drawCall, bool forceRebind)
{
    auto pass = drawCall.pass();
    auto programAndSignature = pass->selectProgram(
        drawCall.variables(), drawCall.targetData(), drawCall.rendererData(), drawCall.rootData()
    );
    auto program = programAndSignature.first;

    if (program == drawCall.program())
        return;

    if (drawCall.program())
    {
        unwatchProgramSignature(
            drawCall,
            drawCall.pass()->macroBindings(),
            drawCall.rootData(),
            drawCall.rendererData(),
            drawCall.targetData()
        );
        unbindDrawCall(drawCall);
    }

    bindDrawCall(drawCall, pass, program, forceRebind);

    if (programAndSignature.second != nullptr)
    {
        watchProgramSignature(
            drawCall,
            drawCall.pass()->macroBindings(),
            drawCall.rootData(),
            drawCall.rendererData(),
            drawCall.targetData()
        );
    }
}

void
DrawCallPool::uniformBindingPropertyAddedHandler(DrawCall&                          drawCall,
                                                 const ProgramInputs::UniformInput& input,
                                                 const data::BindingMap&            uniformBindingMap,
                                                 bool                               forceRebind)
{
    if (!forceRebind && std::find(_invalidDrawCalls.begin(), _invalidDrawCalls.end(), &drawCall) != _invalidDrawCalls.end())
        return;

    data::ResolvedBinding* resolvedBinding = drawCall.bindUniform(
        input, uniformBindingMap.bindings, uniformBindingMap.defaultValues
    );

    if (resolvedBinding != nullptr)
    {
        auto& propertyName = resolvedBinding->propertyName;
        auto propertyExist = resolvedBinding->store.hasProperty(propertyName);
        auto& signal = propertyExist
            ? resolvedBinding->store.propertyRemoved(propertyName)
            : resolvedBinding->store.propertyAdded(propertyName);

        _propChangedSlot[{&resolvedBinding->binding, &drawCall}] = signal.connect(
            [&](data::Store&, data::Provider::Ptr, const std::string&)
            {
                _drawCallToPropRebindFuncs[&drawCall].push_back(std::bind(
                    &DrawCallPool::uniformBindingPropertyAddedHandler,
                    this,
                    std::ref(drawCall),
                    std::ref(input),
                    std::ref(uniformBindingMap),
                    false
                ));
            }
        );

        // If this draw call needs to be sorted
        // => we listen to the useful properties
        if (propertyExist && drawCall.zSorted())
        {
            auto propertyRelatedToZSort = false;
            for (auto i = 0; i < _zSortUsefulPropertyNames.size(); i++)
            {
                if (data::Store::getActualPropertyName(drawCall.variables(), _zSortUsefulPropertyNames[i]) == propertyName)
                    propertyRelatedToZSort = true;
            }

            if (propertyRelatedToZSort)
            {
                // Bind the signal to request a Z-sorting if one of these properties changed
                _zSortUsefulPropertyChangedSlot[{&resolvedBinding->binding, &drawCall}] = resolvedBinding->store.propertyChanged().connect(
                    [&](data::Store&, data::Provider::Ptr, const std::string& propertyName)
                {
                    sortDrawCalls();
                });
            }
        }

        delete resolvedBinding;
    }

    if (input.type == ProgramInputs::Type::sampler2d)
    {
        samplerStatesBindingPropertyAddedHandler(drawCall, input, uniformBindingMap);
    }
}

void
DrawCallPool::samplerStatesBindingPropertyAddedHandler(DrawCall&                          drawCall,
                                                       const ProgramInputs::UniformInput& input,
                                                       const data::BindingMap&            uniformBindingMap)
{
    auto resolvedBindings = drawCall.bindSamplerStates(
        input, uniformBindingMap.bindings, uniformBindingMap.defaultValues
    );

    for (auto resolvedBinding : resolvedBindings)
    {
        if (resolvedBinding != nullptr)
        {
            auto& propertyName = resolvedBinding->propertyName;
            auto& signal = resolvedBinding->store.hasProperty(propertyName)
                ? resolvedBinding->store.propertyRemoved(propertyName)
                : resolvedBinding->store.propertyAdded(propertyName);

            _propChangedSlot[{&resolvedBinding->binding, &drawCall}] = signal.connect(
                [&](data::Store&, data::Provider::Ptr, const std::string&)
                {
                    _drawCallToPropRebindFuncs[&drawCall].push_back(std::bind(
                        &DrawCallPool::samplerStatesBindingPropertyAddedHandler,
                        this,
                        std::ref(drawCall),
                        std::ref(input),
                        std::ref(uniformBindingMap)
                    ));
                }
            );

            delete resolvedBinding;
        }
    }
}

void
DrawCallPool::update()
{
    for (auto* drawCallPtr : _invalidDrawCalls)
        initializeDrawCall(*drawCallPtr, true);

    _invalidDrawCalls.clear();

    for (auto drawCallPtrAndFuncList : _drawCallToPropRebindFuncs)
    {
        for (auto& func : drawCallPtrAndFuncList.second)
            func();
    }

    _drawCallToPropRebindFuncs.clear();
}

void
DrawCallPool::invalidateDrawCalls(const DrawCallIteratorPair&                         iterators,
                                  const std::unordered_map<std::string, std::string>& variables)
{
    auto end = std::next(iterators.second);

    for (auto it = iterators.first; it != end; ++it)
    {
        auto& drawCall = **it;

        _invalidDrawCalls.insert(&drawCall);
        drawCall.variables().clear();
        drawCall.variables().insert(variables.begin(), variables.end());
    }
}

void
DrawCallPool::sortDrawCalls()
{
    _drawCalls.sort(
        std::bind(
            &DrawCallPool::compareDrawCalls,
            this,
            std::placeholders::_1,
            std::placeholders::_2
        )
    );
}

bool
DrawCallPool::compareDrawCalls(DrawCall* a, DrawCall* b)
{
    const float aPriority = a->priority();
    const float bPriority = b->priority();
    const bool samePriority = fabsf(aPriority - bPriority) < 1e-3f;

    if (samePriority)
    {
        if (a->target().id == b->target().id)
        {
            if (a->zSorted() && b->zSorted())
            {
                auto aPosition = a->getEyeSpacePosition();
                auto bPosition = b->getEyeSpacePosition();

                return aPosition.z > bPosition.z;
            }
        }

        return a->target().id < b->target().id;
    }

    return aPriority > bPriority;
}

void
DrawCallPool::clear()
{
    _drawCalls.clear();
    _macroToDrawCalls.clear();
    _invalidDrawCalls.clear();
    _macroChangedSlot.clear();
    _propChangedSlot.clear();
}

void
DrawCallPool::bindDrawCall(DrawCall& drawCall, Pass::Ptr pass, Program::Ptr program, bool forceRebind)
{
    drawCall.bind(program);

    // bind attributes
    // FIXME: like for uniforms, watch and swap default values / binding value
    for (const auto& input : program->inputs().attributes())
        drawCall.bindAttribute(input, pass->attributeBindings().bindings, pass->attributeBindings().defaultValues);

    // bind states
    drawCall.bindStates(pass->stateBindings().bindings, pass->stateBindings().defaultValues);

    // bind uniforms
    for (const auto& input : program->inputs().uniforms())
        uniformBindingPropertyAddedHandler(drawCall, input, pass->uniformBindings(), forceRebind);

    // bind index buffer
    if (!pass->isPostProcessing())
        drawCall.bindIndexBuffer();
}

void
DrawCallPool::unbindDrawCall(DrawCall& drawCall)
{
    std::list<PropertyChangedSlotMap::key_type> toRemove;
    for (auto& bindingDrawCallPairAndSlot : _propChangedSlot)
    {
        if (bindingDrawCallPairAndSlot.first.second == &drawCall)
            toRemove.push_front(bindingDrawCallPairAndSlot.first);
    }

    for (const auto& key : toRemove)
        _propChangedSlot.erase(key);

    _drawCallToPropRebindFuncs.erase(&drawCall);
}
