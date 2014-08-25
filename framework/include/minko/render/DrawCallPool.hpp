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

#pragma once

#include "minko/Common.hpp"

#include "minko/Signal.hpp"
#include "minko/render/Effect.hpp"
#include "minko/render/DrawCall.hpp"
#include "minko/data/Container.hpp"

namespace minko
{
	namespace render
	{
		class DrawCallPool :
			public std::enable_shared_from_this<DrawCallPool>
		{
        private:
            typedef std::list<DrawCall*>::iterator                                          DrawCallIterator;
            typedef data::Container::PropertyChangedSignal                                  PropertyChanged;
            typedef std::pair<PropertyChanged::Slot, uint>                                  ChangedSlot;
            typedef std::list<DrawCall*>                                                    DrawCallList;
            typedef data::Container                                                         Container;
            typedef data::MacroBinding                                                      MacroBinding;
            typedef std::pair<const MacroBinding*, const Container*>                        MacroBindingKey;
            typedef std::pair<const Container*, const PropertyChanged*>                     ContainerKey;
            typedef std::function<void(Container&, const std::string&, const std::string&)> MacroCallback;
            
            template<typename T, typename V>
            struct PtrPairHash
            {
                size_t
                operator()(const std::pair<const T*, const V*>& key) const
                {
                    size_t seed = 0;

                    minko::hash_combine(seed, (void*)(key.first));
                    minko::hash_combine(seed, (void*)(key.second));

                    return seed;
                }
            };

            template<typename T, typename V>
            struct PtrPairEq
            {
                bool
                operator()(const std::pair<const T*, const V*>& lhs,
                           const std::pair<const T*, const V*>& rhs) const
                {
                    return lhs.first == rhs.first && lhs.second == rhs.second;
                }
            };

            typedef PtrPairHash<MacroBinding, Container>                                       BindingHash;
            typedef PtrPairEq<MacroBinding, Container>                                         BindingEq;
            typedef std::unordered_map<MacroBindingKey, DrawCallList, BindingHash, BindingEq>  MacroToDrawCallsMap;
            typedef PtrPairHash<Container, PropertyChanged>                                    ContainerHash;
            typedef PtrPairEq<Container, PropertyChanged>                                      ContainerEq;
            typedef std::unordered_map<ContainerKey, ChangedSlot, ContainerHash, ContainerEq>  MacroToChangedSlotMap;

        public:
            typedef std::pair<DrawCallIterator, DrawCallIterator>   DrawCallIteratorPair;

		private:
            std::list<DrawCall*>            _drawCalls;
            std::set<std::string>           _watchedProperties;
            MacroToDrawCallsMap             _macroToDrawCalls;
            std::unordered_set<DrawCall*>   _changedDrawCalls;
            MacroToChangedSlotMap           _macroChangedSlot;

		public:
            ~DrawCallPool()
            {

            }

			const std::list<DrawCall*>&
            drawCalls()
            {
                return _drawCalls;
            }
			
            DrawCallIteratorPair
            addDrawCalls(std::shared_ptr<Effect>                                effect,
                         const std::unordered_map<std::string, std::string>&    variables,
                         const std::string&                                     techniqueName,
                         data::Container&                                       rootData,
                         data::Container&                                       rendererData,
                         data::Container&                                       targetData);

            void
            removeDrawCalls(const DrawCallIteratorPair& iterators);

            void
            update();

        private:
            void
            watchProgramSignature(DrawCall*                     drawCall,
                                  const data::MacroBindingMap&  macroBindings,
                                  data::Container&              rootData,
                                  data::Container&              rendererData,
                                  data::Container&              targetData);

            void
            unwatchProgramSignature(DrawCall*                     drawCall,
                                    const data::MacroBindingMap&  macroBindings,
                                    data::Container&              rootData,
                                    data::Container&              rendererData,
                                    data::Container&              targetData);

            void
            macroPropertyAddedHandler(data::Container&            container,
                                      const std::list<DrawCall*>& drawCalls);

            void
            macroPropertyRemovedHandler(data::Container&            container,
                                        const std::list<DrawCall*>& drawCalls);

            void
            macroPropertyChangedHandler(const std::list<DrawCall*>& drawCalls);

            void
            initializeDrawCall(DrawCall* drawCall);
            
            void
            addMacroCallback(const ContainerKey&  key,
                             data::Container&     container,
                             const MacroCallback& callback);

            void
            removeMacroCallback(const ContainerKey& key);
		};
	}
}
