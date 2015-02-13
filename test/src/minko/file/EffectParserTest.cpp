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

#include "EffectParserTest.hpp"

#include "minko/MinkoTests.hpp"

using namespace minko;
using namespace minko::render;
using namespace minko::file;

Effect::Ptr
EffectParserTest::loadEffect(const std::string& filename)
{   
    auto lib = AssetLibrary::create(MinkoTests::canvas()->context());

    lib->loader()->queue(filename);
    lib->loader()->load();

    return lib->effect(filename);
}

TEST_F(EffectParserTest, OneAttributeBinding)
{
    auto fx = loadEffect("effect/OneAttributeBinding.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->attributeBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->attributeBindings().bindings.at("aPosition").propertyName, "geometry[${geometryUuid}].position");
    ASSERT_EQ(fx->techniques().at("default")[0]->attributeBindings().bindings.at("aPosition").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, TwoAttributeBindings)
{
    auto fx = loadEffect("effect/TwoAttributeBindings.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->attributeBindings().bindings.size(), 2);
    ASSERT_EQ(fx->techniques().at("default")[0]->attributeBindings().bindings.at("aPosition").propertyName, "geometry[${geometryUuid}].position");
    ASSERT_EQ(fx->techniques().at("default")[0]->attributeBindings().bindings.at("aPosition").source, data::Binding::Source::TARGET);
    ASSERT_EQ(fx->techniques().at("default")[0]->attributeBindings().bindings.at("aUv").propertyName, "geometry[${geometryUuid}].uv");
    ASSERT_EQ(fx->techniques().at("default")[0]->attributeBindings().bindings.at("aUv").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, OneUniformBinding)
{
    auto fx = loadEffect("effect/OneUniformBinding.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().bindings.at("uDiffuseMap").propertyName, "material[${materialUuid}].diffuseMap");
    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().bindings.at("uDiffuseMap").source, data::Binding::Source::TARGET);
}

/*** States ***/

/** State default values **/

TEST_F(EffectParserTest, StatesDefaultValues)
{
    auto fx = loadEffect("effect/StatesDefaultValues.effect");
    auto& states = fx->techniques().at("default")[0]->states();

    ASSERT_EQ(states.priority(), States::DEFAULT_PRIORITY);
    ASSERT_EQ(states.zSorted(), States::DEFAULT_ZSORTED);
    ASSERT_EQ(states.blendingSourceFactor(), States::DEFAULT_BLENDING_SOURCE);
    ASSERT_EQ(states.blendingDestinationFactor(), States::DEFAULT_BLENDING_DESTINATION);
    ASSERT_EQ(states.colorMask(), States::DEFAULT_COLOR_MASK);
    ASSERT_EQ(states.depthMask(), States::DEFAULT_DEPTH_MASK);
    ASSERT_EQ(states.depthFunction(), States::DEFAULT_DEPTH_FUNCTION);
    ASSERT_EQ(states.triangleCulling(), States::DEFAULT_TRIANGLE_CULLING);
    ASSERT_EQ(states.stencilFunction(), States::DEFAULT_STENCIL_FUNCTION);
    ASSERT_EQ(states.stencilReference(), States::DEFAULT_STENCIL_REFERENCE);
    ASSERT_EQ(states.stencilMask(), States::DEFAULT_STENCIL_MASK);
    ASSERT_EQ(states.stencilFailOperation(), States::DEFAULT_STENCIL_FAIL_OP);
    ASSERT_EQ(states.stencilZFailOperation(), States::DEFAULT_STENCIL_ZFAIL_OP);
    ASSERT_EQ(states.stencilZPassOperation(), States::DEFAULT_STENCIL_ZPASS_OP);
    ASSERT_EQ(states.scissorTest(), States::DEFAULT_SCISSOR_TEST);
    ASSERT_EQ(states.scissorBox(), States::DEFAULT_SCISSOR_BOX);

    // FIXME : test render target value
    // FIXME : test render states values
}

/** Priority **/

TEST_F(EffectParserTest, PriorityFloatDefaultValue)
{
    auto fx = loadEffect("effect/PriorityFloatDefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().priority(), 42.f);
}

TEST_F(EffectParserTest, PriorityFloatArrayValue)
{
    auto fx = loadEffect("effect/PriorityFloatArrayValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().priority(), 2042.f);
}

/** Depth mask **/

TEST_F(EffectParserTest, StatesDepthMask)
{
    auto fx = loadEffect("effect/StatesDepthMask.effect");

    auto value = fx->techniques().at("default")[0]->states().depthMask();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthMask(), false);
}

/** Depth function **/

TEST_F(EffectParserTest, StatesDepthFunctionAlways)
{
    auto fx = loadEffect("effect/StatesDepthFunctionAlways.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::ALWAYS);
}

TEST_F(EffectParserTest, StatesDepthFunctionEqual)
{
    auto fx = loadEffect("effect/StatesDepthFunctionEqual.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::EQUAL);
}

TEST_F(EffectParserTest, StatesDepthFunctionGreater)
{
    auto fx = loadEffect("effect/StatesDepthFunctionGreater.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::GREATER);
}

TEST_F(EffectParserTest, StatesDepthFunctionGreaterEqual)
{
    auto fx = loadEffect("effect/StatesDepthFunctionGreaterEqual.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::GREATER_EQUAL);
}

TEST_F(EffectParserTest, StatesDepthFunctionLess)
{
    auto fx = loadEffect("effect/StatesDepthFunctionLess.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::LESS);
}

TEST_F(EffectParserTest, StatesDepthFunctionLessEqual)
{
    auto fx = loadEffect("effect/StatesDepthFunctionLessEqual.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::LESS_EQUAL);
}

TEST_F(EffectParserTest, StatesDepthFunctionNever)
{
    auto fx = loadEffect("effect/StatesDepthFunctionNever.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::NEVER);
}

TEST_F(EffectParserTest, StatesDepthFunctionNotEqual)
{
    auto fx = loadEffect("effect/StatesDepthFunctionNotEqual.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::NOT_EQUAL);
}

/** Triangle Culling **/

TEST_F(EffectParserTest, StatesTriangleCullingBack)
{
    auto fx = loadEffect("effect/StatesTriangleCullingBack.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().triangleCulling(), TriangleCulling::BACK);
}

TEST_F(EffectParserTest, StatesTriangleCullingBoth)
{
    auto fx = loadEffect("effect/StatesTriangleCullingBoth.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().triangleCulling(), TriangleCulling::BOTH);
}

TEST_F(EffectParserTest, StatesTriangleCullingFront)
{
    auto fx = loadEffect("effect/StatesTriangleCullingFront.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().triangleCulling(), TriangleCulling::FRONT);
}

TEST_F(EffectParserTest, StatesTriangleCullingNone)
{
    auto fx = loadEffect("effect/StatesTriangleCullingNone.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().triangleCulling(), TriangleCulling::NONE);
}

/** Blend mode **/

TEST_F(EffectParserTest, StatesBlendingModeDefault)
{
    auto fx = loadEffect("effect/StatesBlendingModeDefault.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingSourceFactor(), Blending::Source::ONE);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingDestinationFactor(), Blending::Destination::ZERO);
}

TEST_F(EffectParserTest, StatesBlendingModeAdditive)
{
    auto fx = loadEffect("effect/StatesBlendingModeAdditive.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingSourceFactor(), Blending::Source::ONE);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingDestinationFactor(), Blending::Destination::ONE);
}

TEST_F(EffectParserTest, StatesBlendingModeAlpha)
{
    auto fx = loadEffect("effect/StatesBlendingModeAlpha.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingSourceFactor(), Blending::Source::SRC_ALPHA);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingDestinationFactor(), Blending::Destination::ONE_MINUS_SRC_ALPHA);
}

TEST_F(EffectParserTest, StatesTarget)
{
    auto assets = file::AssetLibrary::create(MinkoTests::canvas()->context());
    auto fx = MinkoTests::loadEffect("effect/StatesTarget.effect", assets);

    ASSERT_NE(fx, nullptr);
    ASSERT_NE(fx->techniques().at("default")[0]->states().target(), States::DEFAULT_TARGET);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().target(), assets->texture("test-render-target0")->sampler());
    ASSERT_NE(assets->texture("test-render-target0"), nullptr);
    ASSERT_EQ(assets->texture("test-render-target0")->width(), 1024);
    ASSERT_EQ(assets->texture("test-render-target0")->height(), 1024);
    ASSERT_NE(fx->techniques().at("default")[1]->states().target(), States::DEFAULT_TARGET);
    ASSERT_EQ(fx->techniques().at("default")[1]->states().target(), assets->texture("test-render-target1")->sampler());
    ASSERT_NE(assets->texture("test-render-target1"), nullptr);
    ASSERT_EQ(assets->texture("test-render-target1")->width(), 2048);
    ASSERT_EQ(assets->texture("test-render-target1")->height(), 1024);
}

/*** States bindings ***/

TEST_F(EffectParserTest, StatesBindingPriority)
{
    auto fx = loadEffect("effect/StatesBindingPriority.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("priority").propertyName, "material[${materialUuid}].priority");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("priority").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingZSorted)
{
    auto fx = loadEffect("effect/StatesBindingZSorted.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("zSorted").propertyName, "material[${materialUuid}].zSorted");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("zSorted").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingBlendingMode)
{
    auto fx = loadEffect("effect/StatesBindingBlendingMode.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("blendingMode").propertyName, "material[${materialUuid}].blendingMode");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("blendingMode").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingBlendingSource)
{
    auto fx = loadEffect("effect/StatesBindingBlendingSource.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("blendingSource").propertyName, "material[${materialUuid}].blendingSource");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("blendingSource").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingBlendingDestination)
{
    auto fx = loadEffect("effect/StatesBindingBlendingDestination.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("blendingDestination").propertyName, "material[${materialUuid}].blendingDestination");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("blendingDestination").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingColorMask)
{
    auto fx = loadEffect("effect/StatesBindingColorMask.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("colorMask").propertyName, "material[${materialUuid}].colorMask");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("colorMask").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingDepthMask)
{
    auto fx = loadEffect("effect/StatesBindingDepthMask.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("depthMask").propertyName, "material[${materialUuid}].depthMask");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("depthMask").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingDepthFunction)
{
    auto fx = loadEffect("effect/StatesBindingDepthFunction.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("depthFunction").propertyName, "material[${materialUuid}].depthFunction");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("depthFunction").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingTriangleCulling)
{
    auto fx = loadEffect("effect/StatesBindingTriangleCulling.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("triangleCulling").propertyName, "material[${materialUuid}].triangleCulling");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("triangleCulling").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingStencilFunction)
{
    auto fx = loadEffect("effect/StatesBindingStencilFunction.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilFunction").propertyName, "material[${materialUuid}].stencilFunction");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilFunction").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingStencilReference)
{
    auto fx = loadEffect("effect/StatesBindingStencilReference.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilReference").propertyName, "material[${materialUuid}].stencilReference");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilReference").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingStencilMask)
{
    auto fx = loadEffect("effect/StatesBindingStencilMask.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilMask").propertyName, "material[${materialUuid}].stencilMask");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilMask").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingStencilFailOperation)
{
    auto fx = loadEffect("effect/StatesBindingStencilFailOperation.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilFailOperation").propertyName, "material[${materialUuid}].stencilFailOperation");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilFailOperation").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingStencilZFailOperation)
{
    auto fx = loadEffect("effect/StatesBindingStencilZFailOperation.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilZFailOperation").propertyName, "material[${materialUuid}].stencilZFailOperation");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilZFailOperation").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingStencilZPassOperation)
{
    auto fx = loadEffect("effect/StatesBindingStencilZPassOperation.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilZPassOperation").propertyName, "material[${materialUuid}].stencilZPassOperation");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilZPassOperation").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingScissorTest)
{
    auto fx = loadEffect("effect/StatesBindingScissorTest.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("scissorTest").propertyName, "material[${materialUuid}].scissorTest");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("scissorTest").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingScissorBox)
{
    auto fx = loadEffect("effect/StatesBindingScissorBox.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("scissorBox").propertyName, "material[${materialUuid}].scissorBox");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("scissorBox").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingTarget)
{
    auto fx = loadEffect("effect/StatesBindingTarget.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("target").propertyName, "material[${materialUuid}].target");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("target").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, BoolDefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/BoolDefaultValue.effect");

    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().defaultValues.get<int>("testBool1Uniform"), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().defaultValues.get<math::ivec2>("testBool2Uniform"), math::ivec2(1, 0));
    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().defaultValues.get<math::ivec3>("testBool3Uniform"), math::ivec3(1, 0, 1));
    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().defaultValues.get<math::ivec4>("testBool4Uniform"), math::ivec4(1, 0, 1, 0));
}

TEST_F(EffectParserTest, Float4DefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/Float4DefaultValue.effect");

    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().defaultValues.get<math::vec4>("testFloat4Uniform"), math::vec4(1.f));
}

TEST_F(EffectParserTest, OneUniformBindingAndDefault)
{
    auto fx = MinkoTests::loadEffect("effect/OneUniformBindingAndDefault.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().bindings.at("uDiffuseColor").propertyName, "diffuseColor");
    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().bindings.at("uDiffuseColor").source, data::Binding::Source::TARGET);
    ASSERT_TRUE(fx->techniques().at("default")[0]->uniformBindings().defaultValues.hasProperty("uDiffuseColor"));
}

TEST_F(EffectParserTest, MacroIntDefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroIntDefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_INT_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<int>("TEST_INT_MACRO"), 42);
}

TEST_F(EffectParserTest, MacroInt2DefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroInt2DefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_INT2_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<math::ivec2>("TEST_INT2_MACRO"), math::ivec2(42, 23));
}

TEST_F(EffectParserTest, MacroInt3DefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroInt3DefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_INT3_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<math::ivec3>("TEST_INT3_MACRO"), math::ivec3(42, 23, 13));
}

TEST_F(EffectParserTest, MacroInt4DefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroInt4DefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_INT4_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<math::ivec4>("TEST_INT4_MACRO"), math::ivec4(42, 23, 13, 7));
}

TEST_F(EffectParserTest, MacroFloatDefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroFloatDefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_FLOAT_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<float>("TEST_FLOAT_MACRO"), 42.24f);
}

TEST_F(EffectParserTest, MacroFloat2DefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroFloat2DefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_FLOAT2_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<math::vec2>("TEST_FLOAT2_MACRO"), math::vec2(42.24f, 23.32f));
}

TEST_F(EffectParserTest, MacroFloat3DefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroFloat3DefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_FLOAT3_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<math::vec3>("TEST_FLOAT3_MACRO"), math::vec3(42.24f, 23.32f, 13.31f));
}

TEST_F(EffectParserTest, MacroFloat4DefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroFloat4DefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_FLOAT4_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<math::vec4>("TEST_FLOAT4_MACRO"), math::vec4(42.24f, 23.32f, 13.31f, 7.7f));
}

TEST_F(EffectParserTest, MacroBoolDefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroBoolDefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_BOOL_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<int>("TEST_BOOL_MACRO"), 1);
}

TEST_F(EffectParserTest, MacroBool2DefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroBool2DefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_BOOL2_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<math::ivec2>("TEST_BOOL2_MACRO"), math::ivec2(1, 0));
}

TEST_F(EffectParserTest, MacroBool3DefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroBool3DefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_BOOL3_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<math::ivec3>("TEST_BOOL3_MACRO"), math::ivec3(1, 0, 1));
}

TEST_F(EffectParserTest, MacroBool4DefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroBool4DefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_BOOL4_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<math::ivec4>("TEST_BOOL4_MACRO"), math::ivec4(1, 0, 1, 0));
}

TEST_F(EffectParserTest, MultiplePassesHaveDifferentStateData)
{
    auto fx = MinkoTests::loadEffect("effect/MultiplePasses.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 3);
    ASSERT_NE(
        &fx->techniques().at("default")[0]->stateBindings().bindings,
        &fx->techniques().at("default")[1]->stateBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->stateBindings().defaultValues,
        &fx->techniques().at("default")[1]->stateBindings().defaultValues
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->stateBindings().defaultValues.providers().front(),
        &fx->techniques().at("default")[1]->stateBindings().defaultValues.providers().front()
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->stateBindings().bindings,
        &fx->techniques().at("default")[2]->stateBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->stateBindings().defaultValues,
        &fx->techniques().at("default")[2]->stateBindings().defaultValues
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->stateBindings().defaultValues.providers().front(),
        &fx->techniques().at("default")[2]->stateBindings().defaultValues.providers().front()
    );
    ASSERT_NE(
        &fx->techniques().at("default")[1]->stateBindings().bindings,
        &fx->techniques().at("default")[2]->stateBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[1]->stateBindings().defaultValues,
        &fx->techniques().at("default")[2]->stateBindings().defaultValues
    );
    ASSERT_NE(
        &fx->techniques().at("default")[1]->stateBindings().defaultValues.providers().front(),
        &fx->techniques().at("default")[2]->stateBindings().defaultValues.providers().front()
    );
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().defaultValues.providers().size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[1]->stateBindings().defaultValues.providers().size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[2]->stateBindings().defaultValues.providers().size(), 1);
}

TEST_F(EffectParserTest, MultiplePassesHaveDifferentUniformData)
{
    auto fx = MinkoTests::loadEffect("effect/MultiplePasses.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 3);
    ASSERT_NE(
        &fx->techniques().at("default")[0]->uniformBindings().bindings,
        &fx->techniques().at("default")[1]->uniformBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->uniformBindings().defaultValues,
        &fx->techniques().at("default")[1]->uniformBindings().defaultValues
    );
    ASSERT_NE(
        fx->techniques().at("default")[0]->uniformBindings().defaultValues.providers().front(),
        fx->techniques().at("default")[1]->uniformBindings().defaultValues.providers().front()
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->uniformBindings().bindings,
        &fx->techniques().at("default")[2]->uniformBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->uniformBindings().defaultValues,
        &fx->techniques().at("default")[2]->uniformBindings().defaultValues
    );
    ASSERT_NE(
        fx->techniques().at("default")[0]->uniformBindings().defaultValues.providers().front(),
        fx->techniques().at("default")[2]->uniformBindings().defaultValues.providers().front()
    );
    ASSERT_NE(
        &fx->techniques().at("default")[1]->uniformBindings().bindings,
        &fx->techniques().at("default")[2]->uniformBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[1]->uniformBindings().defaultValues,
        &fx->techniques().at("default")[2]->uniformBindings().defaultValues
    );
    ASSERT_NE(
        fx->techniques().at("default")[1]->uniformBindings().defaultValues.providers().front(),
        fx->techniques().at("default")[2]->uniformBindings().defaultValues.providers().front()
    );

    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().defaultValues.providers().size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[1]->uniformBindings().defaultValues.providers().size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[2]->uniformBindings().defaultValues.providers().size(), 1);
}

TEST_F(EffectParserTest, MultiplePassesHaveDifferentMacroData)
{
    auto fx = MinkoTests::loadEffect("effect/MultiplePasses.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 3);
    ASSERT_NE(
        &fx->techniques().at("default")[0]->macroBindings().bindings,
        &fx->techniques().at("default")[1]->macroBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->macroBindings().defaultValues,
        &fx->techniques().at("default")[1]->macroBindings().defaultValues
    );
    ASSERT_NE(
        fx->techniques().at("default")[0]->macroBindings().defaultValues.providers().front(),
        fx->techniques().at("default")[1]->macroBindings().defaultValues.providers().front()
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->macroBindings().bindings,
        &fx->techniques().at("default")[2]->macroBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->macroBindings().defaultValues,
        &fx->techniques().at("default")[2]->macroBindings().defaultValues
    );
    ASSERT_NE(
        fx->techniques().at("default")[0]->macroBindings().defaultValues.providers().front(),
        fx->techniques().at("default")[2]->macroBindings().defaultValues.providers().front()
    );
    ASSERT_NE(
        &fx->techniques().at("default")[1]->macroBindings().bindings,
        &fx->techniques().at("default")[2]->macroBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[1]->macroBindings().defaultValues,
        &fx->techniques().at("default")[2]->macroBindings().defaultValues
    );
    ASSERT_NE(
        fx->techniques().at("default")[1]->macroBindings().defaultValues.providers().front(),
        fx->techniques().at("default")[2]->macroBindings().defaultValues.providers().front()
    );
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.providers().size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[1]->macroBindings().defaultValues.providers().size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[2]->macroBindings().defaultValues.providers().size(), 1);
}

TEST_F(EffectParserTest, MultiplePassesHaveDifferentAttributeData)
{
    auto fx = MinkoTests::loadEffect("effect/MultiplePasses.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 3);
    ASSERT_NE(
        &fx->techniques().at("default")[0]->attributeBindings().bindings,
        &fx->techniques().at("default")[1]->attributeBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->attributeBindings().defaultValues,
        &fx->techniques().at("default")[1]->attributeBindings().defaultValues
    );
    ASSERT_NE(
        fx->techniques().at("default")[0]->attributeBindings().defaultValues.providers().front(),
        fx->techniques().at("default")[1]->attributeBindings().defaultValues.providers().front()
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->attributeBindings().bindings,
        &fx->techniques().at("default")[2]->attributeBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->attributeBindings().defaultValues,
        &fx->techniques().at("default")[2]->attributeBindings().defaultValues
    );
    ASSERT_NE(
        fx->techniques().at("default")[0]->attributeBindings().defaultValues.providers().front(),
        fx->techniques().at("default")[2]->attributeBindings().defaultValues.providers().front()
    );
    ASSERT_NE(
        &fx->techniques().at("default")[1]->attributeBindings().bindings,
        &fx->techniques().at("default")[2]->attributeBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[1]->attributeBindings().defaultValues,
        &fx->techniques().at("default")[2]->attributeBindings().defaultValues
    );
    ASSERT_NE(
        fx->techniques().at("default")[1]->attributeBindings().defaultValues.providers().front(),
        fx->techniques().at("default")[2]->attributeBindings().defaultValues.providers().front()
    );
    ASSERT_EQ(fx->techniques().at("default")[0]->attributeBindings().defaultValues.providers().size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[1]->attributeBindings().defaultValues.providers().size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[2]->attributeBindings().defaultValues.providers().size(), 1);

}

/** Sampler states **/

TEST_F(EffectParserTest, SamplerStatesWrapModeClamp)
{
    auto fx = loadEffect("effect/SamplerStatesWrapModeClamp.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(
        fx->techniques().at("default")[0]->uniformBindings().defaultValues.get<WrapMode>(
            SamplerStates::uniformNameToSamplerStateName(
                "diffuseMap", 
                SamplerStates::PROPERTY_WRAP_MODE
            )
        ), 
        WrapMode::CLAMP
    );
}

TEST_F(EffectParserTest, SamplerStatesWrapModeRepeat)
{
    auto fx = loadEffect("effect/SamplerStatesWrapModeRepeat.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(
        fx->techniques().at("default")[0]->uniformBindings().defaultValues.get<WrapMode>(
            SamplerStates::uniformNameToSamplerStateName(
                "diffuseMap", 
                SamplerStates::PROPERTY_WRAP_MODE
            )
        ),
        WrapMode::REPEAT
    );
}

TEST_F(EffectParserTest, SamplerStatesTextureFilterLinear)
{
    auto fx = loadEffect("effect/SamplerStatesTextureFilterLinear.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(
        fx->techniques().at("default")[0]->uniformBindings().defaultValues.get<TextureFilter>(
            SamplerStates::uniformNameToSamplerStateName(
                "diffuseMap", 
                SamplerStates::PROPERTY_TEXTURE_FILTER
            )
        ),
        TextureFilter::LINEAR
    );
}

TEST_F(EffectParserTest, SamplerStatesTextureFilterNearest)
{
    auto fx = loadEffect("effect/SamplerStatesTextureFilterNearest.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(
        fx->techniques().at("default")[0]->uniformBindings().defaultValues.get<TextureFilter>(
            SamplerStates::uniformNameToSamplerStateName(
                "diffuseMap", 
                SamplerStates::PROPERTY_TEXTURE_FILTER
            )
        ),
        TextureFilter::NEAREST
    );
}

TEST_F(EffectParserTest, SamplerStatesMipFilterNone)
{
    auto fx = loadEffect("effect/SamplerStatesMipFilterNone.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(
        fx->techniques().at("default")[0]->uniformBindings().defaultValues.get<MipFilter>(
        SamplerStates::uniformNameToSamplerStateName(
                "diffuseMap", 
                SamplerStates::PROPERTY_MIP_FILTER
            )
        ),
        MipFilter::NONE
    );
}

TEST_F(EffectParserTest, SamplerStatesMipFilterLinear)
{
    auto fx = loadEffect("effect/SamplerStatesMipFilterLinear.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(
        fx->techniques().at("default")[0]->uniformBindings().defaultValues.get<MipFilter>(
        SamplerStates::uniformNameToSamplerStateName(
                "diffuseMap", 
                SamplerStates::PROPERTY_MIP_FILTER
            )
        ),
        MipFilter::LINEAR
    );
}

TEST_F(EffectParserTest, SamplerStatesMipFilterNearest)
{
    auto fx = loadEffect("effect/SamplerStatesMipFilterNearest.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(
        fx->techniques().at("default")[0]->uniformBindings().defaultValues.get<MipFilter>(
        SamplerStates::uniformNameToSamplerStateName(
                "diffuseMap", 
                SamplerStates::PROPERTY_MIP_FILTER
            )
        ),
        MipFilter::NEAREST
    );
}

TEST_F(EffectParserTest, SamplerStatesBindingWrapMode)
{
    auto fx = loadEffect("effect/SamplerStatesBindingWrapMode.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);

    auto uniformBindings = fx->techniques().at("default")[0]->uniformBindings().bindings;

    auto wrapModeUniformName = SamplerStates::uniformNameToSamplerStateName(
        "diffuseMap",
        SamplerStates::PROPERTY_WRAP_MODE
    );

    ASSERT_FALSE(uniformBindings.count(wrapModeUniformName) == 0);
    ASSERT_EQ(
        uniformBindings.at(wrapModeUniformName).propertyName,
        "material[${materialUuid}].diffuseMapWrapMode"
    );
}

TEST_F(EffectParserTest, SamplerStatesBindingTextureFilter)
{
    auto fx = loadEffect("effect/SamplerStatesBindingTextureFilter.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);

    auto uniformBindings = fx->techniques().at("default")[0]->uniformBindings().bindings;

    auto textureFilterUniformName = SamplerStates::uniformNameToSamplerStateName(
        "diffuseMap",
        SamplerStates::PROPERTY_TEXTURE_FILTER
    );

    ASSERT_FALSE(uniformBindings.count(textureFilterUniformName) == 0);
    ASSERT_EQ(
        uniformBindings.at(textureFilterUniformName).propertyName,
        "material[${materialUuid}].diffuseMapTextureFilter"
    );
}

TEST_F(EffectParserTest, SamplerStatesBindingMipFilter)
{
    auto fx = loadEffect("effect/SamplerStatesBindingMipFilter.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);

    auto uniformBindings = fx->techniques().at("default")[0]->uniformBindings().bindings;

    auto mipFilterUniformName = SamplerStates::uniformNameToSamplerStateName(
        "diffuseMap",
        SamplerStates::PROPERTY_MIP_FILTER
    );

    ASSERT_FALSE(uniformBindings.count(mipFilterUniformName) == 0);
    ASSERT_EQ(
        uniformBindings.at(mipFilterUniformName).propertyName,
        "material[${materialUuid}].diffuseMapMipFilter"
    );
}

/* Sampler states binding with default value */

// Wrap mode

TEST_F(EffectParserTest, SamplerStatesBindingWrapModeWithDefaultValueClamp)
{
    auto fx = loadEffect("effect/SamplerStatesBindingWrapModeWithDefaultValueClamp.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);

    auto uniformBindingMap = fx->techniques().at("default")[0]->uniformBindings();

    auto wrapModeUniformName = SamplerStates::uniformNameToSamplerStateName(
        "uDiffuseMap",
        SamplerStates::PROPERTY_WRAP_MODE
    );

    // Check binding
    ASSERT_FALSE(uniformBindingMap.bindings.count(wrapModeUniformName) == 0);
    ASSERT_EQ(
        uniformBindingMap.bindings.at(wrapModeUniformName).propertyName,
        "material[${materialUuid}].diffuseMapWrapMode"
    );

    // Check default value
    ASSERT_TRUE(uniformBindingMap.defaultValues.hasProperty(wrapModeUniformName));

    ASSERT_EQ(
        uniformBindingMap.defaultValues.get<WrapMode>(wrapModeUniformName),
        WrapMode::CLAMP
    );
}

TEST_F(EffectParserTest, SamplerStatesBindingWrapModeWithDefaultValueRepeat)
{
    auto fx = loadEffect("effect/SamplerStatesBindingWrapModeWithDefaultValueRepeat.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);

    auto uniformBindingMap = fx->techniques().at("default")[0]->uniformBindings();

    auto wrapModeUniformName = SamplerStates::uniformNameToSamplerStateName(
        "uDiffuseMap",
        SamplerStates::PROPERTY_WRAP_MODE
    );

    // Check binding
    ASSERT_FALSE(uniformBindingMap.bindings.count(wrapModeUniformName) == 0);
    ASSERT_EQ(
        uniformBindingMap.bindings.at(wrapModeUniformName).propertyName,
        "material[${materialUuid}].diffuseMapWrapMode"
        );

    // Check default value
    ASSERT_TRUE(uniformBindingMap.defaultValues.hasProperty(wrapModeUniformName));

    ASSERT_EQ(
        uniformBindingMap.defaultValues.get<WrapMode>(wrapModeUniformName),
        WrapMode::REPEAT
    );
}

// Texture filter

TEST_F(EffectParserTest, SamplerStatesBindingTextureFilterWithDefaultValueLinear)
{
    auto fx = loadEffect("effect/SamplerStatesBindingTextureFilterWithDefaultValueLinear.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);

    auto uniformBindingMap = fx->techniques().at("default")[0]->uniformBindings();

    auto textureFilterUniformName = SamplerStates::uniformNameToSamplerStateName(
        "uDiffuseMap",
        SamplerStates::PROPERTY_TEXTURE_FILTER
    );

    // Check binding
    ASSERT_FALSE(uniformBindingMap.bindings.count(textureFilterUniformName) == 0);
    ASSERT_EQ(
        uniformBindingMap.bindings.at(textureFilterUniformName).propertyName,
        "material[${materialUuid}].diffuseMapTextureFilter"
    );

    // Check default value
    ASSERT_TRUE(uniformBindingMap.defaultValues.hasProperty(textureFilterUniformName));

    ASSERT_EQ(
        uniformBindingMap.defaultValues.get<TextureFilter>(textureFilterUniformName),
        TextureFilter::LINEAR
    );
}

TEST_F(EffectParserTest, SamplerStatesBindingTextureFilterWithDefaultValueNearest)
{
    auto fx = loadEffect("effect/SamplerStatesBindingTextureFilterWithDefaultValueNearest.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);

    auto uniformBindingMap = fx->techniques().at("default")[0]->uniformBindings();

    auto textureFilterUniformName = SamplerStates::uniformNameToSamplerStateName(
        "uDiffuseMap",
        SamplerStates::PROPERTY_TEXTURE_FILTER
    );

    // Check binding
    ASSERT_FALSE(uniformBindingMap.bindings.count(textureFilterUniformName) == 0);
    ASSERT_EQ(
        uniformBindingMap.bindings.at(textureFilterUniformName).propertyName,
        "material[${materialUuid}].diffuseMapTextureFilter"
    );

    // Check default value
    ASSERT_TRUE(uniformBindingMap.defaultValues.hasProperty(textureFilterUniformName));

    ASSERT_EQ(
        uniformBindingMap.defaultValues.get<TextureFilter>(textureFilterUniformName),
        TextureFilter::NEAREST
    );
}

// Mip filter

TEST_F(EffectParserTest, SamplerStatesBindingMipFilterWithDefaultValueLinear)
{
    auto fx = loadEffect("effect/SamplerStatesBindingMipFilterWithDefaultValueLinear.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);

    auto uniformBindingMap = fx->techniques().at("default")[0]->uniformBindings();

    auto mipFilterUniformName = SamplerStates::uniformNameToSamplerStateName(
        "uDiffuseMap",
        SamplerStates::PROPERTY_MIP_FILTER
    );

    // Check binding
    ASSERT_FALSE(uniformBindingMap.bindings.count(mipFilterUniformName) == 0);
    ASSERT_EQ(
        uniformBindingMap.bindings.at(mipFilterUniformName).propertyName,
        "material[${materialUuid}].diffuseMapMipFilter"
    );

    // Check default value
    ASSERT_TRUE(uniformBindingMap.defaultValues.hasProperty(mipFilterUniformName));

    ASSERT_EQ(
        uniformBindingMap.defaultValues.get<MipFilter>(mipFilterUniformName),
        MipFilter::LINEAR
    );
}

TEST_F(EffectParserTest, SamplerStatesBindingMipFilterWithDefaultValueNearest)
{
    auto fx = loadEffect("effect/SamplerStatesBindingMipFilterWithDefaultValueNearest.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);

    auto uniformBindingMap = fx->techniques().at("default")[0]->uniformBindings();

    auto mipFilterUniformName = SamplerStates::uniformNameToSamplerStateName(
        "uDiffuseMap",
        SamplerStates::PROPERTY_MIP_FILTER
    );

    // Check binding
    ASSERT_FALSE(uniformBindingMap.bindings.count(mipFilterUniformName) == 0);
    ASSERT_EQ(
        uniformBindingMap.bindings.at(mipFilterUniformName).propertyName,
        "material[${materialUuid}].diffuseMapMipFilter"
    );

    // Check default value
    ASSERT_TRUE(uniformBindingMap.defaultValues.hasProperty(mipFilterUniformName));

    ASSERT_EQ(
        uniformBindingMap.defaultValues.get<MipFilter>(mipFilterUniformName),
        MipFilter::NEAREST
    );
}

TEST_F(EffectParserTest, SamplerStatesBindingMipFilterWithDefaultValueNone)
{
    auto fx = loadEffect("effect/SamplerStatesBindingMipFilterWithDefaultValueNone.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);

    auto uniformBindingMap = fx->techniques().at("default")[0]->uniformBindings();

    auto mipFilterUniformName = SamplerStates::uniformNameToSamplerStateName(
        "uDiffuseMap",
        SamplerStates::PROPERTY_MIP_FILTER
    );

    // Check binding
    ASSERT_FALSE(uniformBindingMap.bindings.count(mipFilterUniformName) == 0);
    ASSERT_EQ(
        uniformBindingMap.bindings.at(mipFilterUniformName).propertyName,
        "material[${materialUuid}].diffuseMapMipFilter"
    );

    // Check default value
    ASSERT_TRUE(uniformBindingMap.defaultValues.hasProperty(mipFilterUniformName));

    ASSERT_EQ(
        uniformBindingMap.defaultValues.get<MipFilter>(mipFilterUniformName),
        MipFilter::NONE
    );
}