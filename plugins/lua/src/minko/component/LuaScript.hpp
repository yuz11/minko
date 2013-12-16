/*
Copyright (c) 2013 Aerys

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

#include "minko/component/AbstractScript.hpp"

class LuaGlue;
class LuaGlueClassBase;

namespace minko
{
    namespace component
    {
        class LuaScript :
            public AbstractScript
        {
        public:
            typedef std::shared_ptr<LuaScript>  Ptr;

        private:
            typedef std::shared_ptr<scene::Node>    NodePtr;

            class LuaSignal
            {

            };

            class LuaStub
            {
            private:
                NodePtr _target;

            public:
                LuaStub(NodePtr target) : _target(target) {}

                std::shared_ptr<component::SceneManager>
                getSceneManager();

                std::shared_ptr<input::Mouse>
                getMouse();

                std::shared_ptr<file::AssetLibrary>
                getAssetLibrary();

                std::shared_ptr<math::Matrix4x4>
                getModelToWorldMatrix(std::shared_ptr<scene::Node> node = nullptr);
            };

        private:
            static bool                             _initialized;
            static LuaGlue                          _state;

            std::string                             _scriptName;
            LuaGlueClassBase*                       _class;
            std::unordered_map<NodePtr, LuaStub*>   _targetToStub;

            bool                                    _hasStartMethod;
            bool                                    _hasUpdateMethod;
            bool                                    _hasStopMethod;

        public:
            static inline
            Ptr
            create(const std::string& name, const std::string& script)
            {
                auto s = std::shared_ptr<LuaScript>(new LuaScript(name));

                s->initialize();
                s->loadScript(script);

                return s;
            }

        protected:
            virtual
			void
			start(NodePtr target);

			virtual
			void
			update(NodePtr target);

            virtual
            void
            stop(NodePtr target);

            void
            initialize();

            void
            loadScript(const std::string& script);

        private:
            LuaScript(const std::string& name);

            static
            void
            initializeLuaBindings();
        };
    }
}
