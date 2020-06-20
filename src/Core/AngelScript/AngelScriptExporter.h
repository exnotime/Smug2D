#pragma once
#include <angelscript.h>
#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>
#include <fstream>
#include <map>

#include <AngelScript/scriptstdstring/scriptstdstring.h>

using namespace AngelScript;
using namespace rapidjson;

namespace AngelScriptExporter {

	std::string FormatType(int typeID, asIScriptEngine* engine) {
		if (typeID < asTYPEID_OBJHANDLE) {
			switch (typeID) {
			case asTYPEID_VOID:
				return "void";
				break;
			case asTYPEID_BOOL:
				return "bool";
				break;
			case asTYPEID_INT8:
				return "int8";
				break;
			case asTYPEID_INT16:
				return "int16";
				break;
			case asTYPEID_INT32:
				return "int";
				break;
			case asTYPEID_INT64:
				return "int64";
				break;
			case asTYPEID_UINT8:
				return "uint8";
				break;
			case asTYPEID_UINT16:
				return "uint16";
				break;
			case asTYPEID_UINT32:
				return "uint";
				break;
			case asTYPEID_UINT64:
				return "uint64";
				break;
			case asTYPEID_FLOAT:
				return "float";
				break;
			case asTYPEID_DOUBLE:
				return "double";
				break;
			}
		}
		//not a builtin type
		asITypeInfo* type = engine->GetTypeInfoById(typeID);
		std::string name = type->GetName();
		if (typeID & asTYPEID_OBJHANDLE) {
			name += "@";
		}
		if (typeID & asTYPEID_HANDLETOCONST) {
			name = "const " + name;
		}
		

		return name;
	}

	std::string FormatBehaviour(asEBehaviours b) {
		std::string ret;
		switch (b)
		{
		case AngelScript::asBEHAVE_CONSTRUCT:
			ret = "Construct";
			break;
		case AngelScript::asBEHAVE_LIST_CONSTRUCT:
			ret = "ListConstruct";
			break;
		case AngelScript::asBEHAVE_DESTRUCT:
			ret = "Destruct";
			break;
		case AngelScript::asBEHAVE_FACTORY:
			ret = "Factory";
			break;
		case AngelScript::asBEHAVE_LIST_FACTORY:
			ret = "ListFactory";
			break;
		case AngelScript::asBEHAVE_ADDREF:
			ret = "AddRef";
			break;
		case AngelScript::asBEHAVE_RELEASE:
			ret = "Release";
			break;
		case AngelScript::asBEHAVE_GET_WEAKREF_FLAG:
			ret = "GetWeakRef";
			break;
		case AngelScript::asBEHAVE_TEMPLATE_CALLBACK:
			ret = "TemplateCallback";
			break;
		case AngelScript::asBEHAVE_FIRST_GC:
			ret = "FirstGC";
			break;
		case AngelScript::asBEHAVE_SETGCFLAG:
			ret = "SetGCFlag";
			break;
		case AngelScript::asBEHAVE_GETGCFLAG:
			ret = "GetGCFlag";
			break;
		case AngelScript::asBEHAVE_ENUMREFS:
			ret = "EnumRefs";
			break;
		case AngelScript::asBEHAVE_RELEASEREFS:
			ret = "ReleaseRefs";
			break;
		case AngelScript::asBEHAVE_MAX:
			break;
		default:
			break;
		}
		return ret;
	}

	Value FormatFunction(asIScriptFunction* asFunc, asIScriptEngine* engine, MemoryPoolAllocator<>& allocator) {
		Value func = Value(kObjectType);
		func.AddMember("Name", Value(asFunc->GetName(), allocator), allocator);
		func.AddMember("Namespace", Value(asFunc->GetNamespace(), allocator), allocator);
		Value params = Value(kArrayType);
		uint32_t paramsCount = asFunc->GetParamCount();
		for (uint32_t k = 0; k < paramsCount; ++k) {
			int typeId = 0;
			asDWORD flags = 0;
			const char* name;
			const char* defaultVal;
			asFunc->GetParam(k, &typeId, &flags, &name, &defaultVal);
			if (typeId == -1) {
				continue;
			}
			std::string typeName = FormatType(typeId, engine);
			Value param = Value(kObjectType);
			param.AddMember("Name", name ? Value(name, allocator) : Value(""), allocator);
			param.AddMember("Type", Value(typeName.c_str(), allocator), allocator);
			param.AddMember("DefaultVal", defaultVal ? Value(defaultVal, allocator) : Value(""), allocator);
			params.PushBack(param, allocator);
		}
		func.AddMember("Declaration", Value(asFunc->GetDeclaration(false), allocator), allocator);
		func.AddMember("Params", params, allocator);
		std::string returnType = FormatType(asFunc->GetReturnTypeId(), engine);
		func.AddMember("ReturnType", Value(returnType.c_str(), allocator), allocator);
		return func;
	}


	void ExportEngineAsJSON(const char* file, asIScriptEngine* engine) {
		engine->SetEngineProperty(asEP_EXPAND_DEF_ARRAY_TO_TMPL, true);
		Document output;
		output.SetObject();
		//global functions
		Value globalFuncs = Value(kArrayType); 
		uint32_t globalFuncCount = engine->GetGlobalFunctionCount();
		for (uint32_t i = 0; i < globalFuncCount; ++i) {
			asIScriptFunction* asFunc = engine->GetGlobalFunctionByIndex(i);
			Value func = FormatFunction(asFunc, engine, output.GetAllocator());
			globalFuncs.PushBack(func, output.GetAllocator());
		}
		output.AddMember("GlobalFunctions", globalFuncs, output.GetAllocator());

		//types
		uint32_t typeCount = engine->GetObjectTypeCount();
		Value globalTypes = Value(kArrayType);
		for (uint32_t i = 0; i < typeCount; ++i) {
			Value type = Value(kObjectType);
			asITypeInfo* asType = engine->GetObjectTypeByIndex(i);
			std::string name = asType->GetName();
			type.AddMember("Name", Value(name.c_str(), output.GetAllocator()), output.GetAllocator());
			type.AddMember("Namespace", Value(asType->GetNamespace(), output.GetAllocator()), output.GetAllocator());
			type.AddMember("AccessMask", Value((uint32_t)asType->GetAccessMask()), output.GetAllocator());
			type.AddMember("Flags", Value((uint32_t)asType->GetFlags() & asOBJ_MASK_VALID_FLAGS), output.GetAllocator());
			type.AddMember("Declaration", Value(engine->GetTypeDeclaration(asType->GetTypeId()), output.GetAllocator()), output.GetAllocator());
			//properties
			uint32_t propCount = asType->GetPropertyCount();
			Value props = Value(kArrayType);
			for (uint32_t k = 0; k < propCount; ++k) {
				const char* name;
				bool isPrivate;
				bool isProtected;
				int typeID;
				asType->GetProperty(k, &name, &typeID, &isPrivate, &isProtected);
				Value prop = Value(kObjectType);
				prop.AddMember("Name", name ? Value(name, output.GetAllocator()) : Value(""), output.GetAllocator());
				std::string typeName = FormatType(typeID, engine).c_str();
				prop.AddMember("Type", Value(typeName.c_str(), output.GetAllocator()), output.GetAllocator());
				prop.AddMember("Private", Value(isPrivate), output.GetAllocator());
				prop.AddMember("Protected", Value(isProtected), output.GetAllocator());
				prop.AddMember("Declaration", Value(asType->GetPropertyDeclaration(k), output.GetAllocator()), output.GetAllocator());
				props.PushBack(prop, output.GetAllocator());
			}
			type.AddMember("Properties", props, output.GetAllocator());
			
			//behaviors
			uint32_t behaviourCount = asType->GetBehaviourCount();
			Value behaviours = Value(kArrayType);
			for (uint32_t k = 0; k < behaviourCount; ++k) {
				asEBehaviours behaviour;
				asIScriptFunction* asFunc =  asType->GetBehaviourByIndex(k, &behaviour);
				Value behave = Value(kObjectType);
				behave.AddMember("Type", Value(behaviour), output.GetAllocator());
				if (asFunc) {
					Value func = FormatFunction(asFunc, engine, output.GetAllocator());
					std::string decl = func["Declaration"].GetString();

					if (behaviour == asBEHAVE_CONSTRUCT){
						decl = std::string("void ") + decl;
					} else if (behaviour == asBEHAVE_DESTRUCT) {
						decl.erase(0, 1);
						decl = std::string("void ") + decl;
					}

					size_t n = decl.find("$");
					if (n != std::string::npos)
						decl[n] = ' ';

					func["Declaration"].SetString(decl.c_str(), output.GetAllocator());
					behave.AddMember("Func", func, output.GetAllocator());
				}
				behaviours.PushBack(behave, output.GetAllocator());
			}
			//factory behaviours
			uint32_t factoryCount = asType->GetFactoryCount();
			for (uint32_t k = 0; k < factoryCount; ++k) {
				asIScriptFunction* asFunc = asType->GetFactoryByIndex(k);
				if (asFunc) {
					Value func = FormatFunction(asFunc, engine, output.GetAllocator());
					Value behave = Value(kObjectType);
					behave.AddMember("Type", Value(asBEHAVE_FACTORY), output.GetAllocator());
					std::string decl = func["Declaration"].GetString();

					size_t n = decl.find("$");
					if (n != std::string::npos)
						decl[n] = ' ';

					func["Declaration"].SetString(decl.c_str(), output.GetAllocator());
					behave.AddMember("Func", func, output.GetAllocator());
					behaviours.PushBack(behave, output.GetAllocator());
				}
			}
			type.AddMember("Behaviors", behaviours, output.GetAllocator());
			//methods
			uint32_t methodCount = asType->GetMethodCount();
			Value methods = Value(kArrayType);
			for (uint32_t k = 0; k < methodCount; ++k) {
				asIScriptFunction* asFunc = asType->GetMethodByIndex(k);
				if (asFunc) {
					Value func = FormatFunction(asFunc, engine, output.GetAllocator());
					methods.PushBack(func, output.GetAllocator());
				}
			}
			type.AddMember("Methods", methods, output.GetAllocator());
			globalTypes.PushBack(type, output.GetAllocator());
		}
		output.AddMember("GlobalTypes", globalTypes, output.GetAllocator());
		//func defs
		uint32_t funcdefCount = engine->GetFuncdefCount();
		Value globalFuncDefs = Value(kArrayType);
		for (uint32_t i = 0; i < funcdefCount; ++i) {
			Value funcdef = Value(kObjectType);
			asITypeInfo* asType = engine->GetFuncdefByIndex(i);
			asIScriptFunction* func =  asType->GetFuncdefSignature();
			funcdef.AddMember("Declaration", Value(func->GetDeclaration(), output.GetAllocator()), output.GetAllocator());
			const char* name = func->GetName();
			funcdef.AddMember("Name", name ? Value(name, output.GetAllocator()) : Value(""), output.GetAllocator());
			const char* namespaceVal = func->GetNamespace();
			funcdef.AddMember("Namespace", namespaceVal ? Value(namespaceVal, output.GetAllocator()) : Value(""), output.GetAllocator());
			globalFuncDefs.PushBack(funcdef, output.GetAllocator());
		}
		output.AddMember("GlobalFuncDefs", globalFuncDefs, output.GetAllocator());
		//typedefs
		uint32_t typedefCount = engine->GetTypedefCount();
		Value globalTypedefs = Value(kArrayType);
		for (uint32_t i = 0; i < typedefCount; ++i) {
			Value typeDef = Value(kObjectType);
			asITypeInfo* asType = engine->GetTypedefByIndex(i);
			const char* name = asType->GetName();
			typeDef.AddMember("Name", name ? Value(name, output.GetAllocator()) : Value(""), output.GetAllocator());
			uint32_t typeID = asType->GetTypedefTypeId();
			std::string type = FormatType(typeID, engine);
			typeDef.AddMember("Type", Value(type.c_str(), output.GetAllocator()), output.GetAllocator());
			const char* namespaceVal = asType->GetNamespace();
			typeDef.AddMember("Namespace", namespaceVal ? Value(namespaceVal, output.GetAllocator()) : Value(""), output.GetAllocator());
			globalTypedefs.PushBack(typeDef, output.GetAllocator());
		}
		output.AddMember("GlobalTypeDefs", globalTypedefs, output.GetAllocator());
		//enums
		uint32_t enumCount = engine->GetEnumCount();
		Value globalEnums = Value(kArrayType);
		for (uint32_t i = 0; i < enumCount; ++i) {
			Value enumVal = Value(kObjectType);
			asITypeInfo* asType = engine->GetEnumByIndex(i);
			uint32_t enumValCount = asType->GetEnumValueCount();
			Value values = Value(kArrayType);
			for (uint32_t k = 0; k < enumValCount; ++k) {
				int val;
				const char* name = asType->GetEnumValueByIndex(k, &val);
				Value prop = Value(kObjectType);
				prop.AddMember("Name", name ? Value(name, output.GetAllocator()) : Value(""), output.GetAllocator());
				prop.AddMember("Value", Value(val), output.GetAllocator());
				values.PushBack(prop, output.GetAllocator());
			}
			enumVal.AddMember("Values", values, output.GetAllocator());
			enumVal.AddMember("Name", Value(asType->GetName(), output.GetAllocator()), output.GetAllocator());
			globalEnums.PushBack(enumVal, output.GetAllocator());
		}
		output.AddMember("GlobalEnums", globalEnums, output.GetAllocator());
		//properties
		uint32_t propCount = engine->GetGlobalPropertyCount();
		Value globalProps = Value(kArrayType);
		for (uint32_t i = 0; i < propCount; ++i) {
			const char* name;
			const char* namespaceVal;
			bool isConst;
			int typeID;
			engine->GetGlobalPropertyByIndex(i, &name, &namespaceVal, &typeID, &isConst);
			Value prop = Value(kObjectType);
			prop.AddMember("Name", name ? Value(name, output.GetAllocator()) : Value(""), output.GetAllocator());
			prop.AddMember("Namespace", namespaceVal ? Value(namespaceVal, output.GetAllocator()) : Value(""), output.GetAllocator());
			std::string typeName = FormatType(typeID, engine).c_str();
			prop.AddMember("Type", Value(typeName.c_str(), output.GetAllocator()), output.GetAllocator());
			prop.AddMember("Const", Value(isConst), output.GetAllocator());
			prop.AddMember("Declaration", Value(engine->GetTypeDeclaration(typeID), output.GetAllocator()), output.GetAllocator());
			globalProps.PushBack(prop, output.GetAllocator());
		}
		output.AddMember("GlobalProperties", globalProps, output.GetAllocator());
		//String factory type
		{
			int stringTypeID = engine->GetStringFactoryReturnTypeId();
			asITypeInfo* stringType = engine->GetTypeInfoById(stringTypeID);
			if (stringType) {
				const char* name = stringType->GetName();
				if (name) {
					output.AddMember("StringFactoryType", Value(name, output.GetAllocator()), output.GetAllocator());
				}
			}
		}
		//Default Array type
		{
			int arrayTypeID = engine->GetDefaultArrayTypeId();
			asITypeInfo* arrayType = engine->GetTypeInfoById(arrayTypeID);
			if (arrayType) {
				const char* name = engine->GetTypeDeclaration(arrayTypeID);
				if (name) {
					output.AddMember("DefaultArrayType", Value(name, output.GetAllocator()), output.GetAllocator());
				}
			}
		}

		//write to file
		std::ofstream outFile(file);
		OStreamWrapper wrapper(outFile);
		Writer<OStreamWrapper> writer(wrapper);
		output.Accept(writer);
	}
	//Copy of the internal string factory from scriptstdstring.cpp
	//Will only be used for string constants during compile
	class StringFactory : public asIStringFactory {
	public:
		StringFactory() {}
		const void* GetStringConstant(const char* data, asUINT length) {
			std::string str(data, length);
			auto& it = stringCache.find(str);
			if (it != stringCache.end())
				it->second++;
			else
				it = stringCache.insert(std::map<std::string, int>::value_type(str, 1)).first;

			return reinterpret_cast<const void*>(&it->first);
		}
		int ReleaseStringConstant(const void* str) {
			int ret = asSUCCESS;

			std::map<std::string, int>::iterator it = stringCache.find(*reinterpret_cast<const std::string*>(str));
			if (it == stringCache.end())
				ret = asERROR;
			else {
				it->second--;
				if (it->second == 0)
					stringCache.erase(it);
			}

			return ret;
		};
		int GetRawStringData(const void* str, char* data, asUINT* length) const {
			if (str == 0)
				return asERROR;

			if (length)
				*length = (asUINT)reinterpret_cast<const std::string*>(str)->length();

			if (data)
				memcpy(data, reinterpret_cast<const std::string*>(str)->c_str(), reinterpret_cast<const std::string*>(str)->length());
			return asSUCCESS;
		}
	private:
		std::map<std::string, int> stringCache;
	};
	static StringFactory* dummyStringFactory = nullptr;

	class DummyStringFactoryCleaner {
	public:
		~DummyStringFactoryCleaner() {
			if (dummyStringFactory) {
				delete dummyStringFactory;
				dummyStringFactory = 0;
			}
		}
	};

	static DummyStringFactoryCleaner cleaner;
	//This function is here to allow arrays to be used. copied from scriptrarray.cpp
	static bool ScriptArrayTemplateCallback(asITypeInfo* ti, bool& dontGarbageCollect) {
		// Make sure the subtype can be instantiated with a default factory/constructor,
		// otherwise we won't be able to instantiate the elements.
		int typeId = ti->GetSubTypeId();
		if (typeId == asTYPEID_VOID)
			return false;
		if ((typeId & asTYPEID_MASK_OBJECT) && !(typeId & asTYPEID_OBJHANDLE)) {
			asITypeInfo* subtype = ti->GetEngine()->GetTypeInfoById(typeId);
			asDWORD flags = subtype->GetFlags();
			if ((flags & asOBJ_VALUE) && !(flags & asOBJ_POD)) {
				// Verify that there is a default constructor
				bool found = false;
				for (asUINT n = 0; n < subtype->GetBehaviourCount(); n++) {
					asEBehaviours beh;
					asIScriptFunction* func = subtype->GetBehaviourByIndex(n, &beh);
					if (beh != asBEHAVE_CONSTRUCT) continue;

					if (func->GetParamCount() == 0) {
						// Found the default constructor
						found = true;
						break;
					}
				}

				if (!found) {
					// There is no default constructor
					// TODO: Should format the message to give the name of the subtype for better understanding
					ti->GetEngine()->WriteMessage("array", 0, 0, asMSGTYPE_ERROR, "The subtype has no default constructor");
					return false;
				}
			} else if ((flags & asOBJ_REF)) {
				bool found = false;

				// If value assignment for ref type has been disabled then the array
				// can be created if the type has a default factory function
				if (!ti->GetEngine()->GetEngineProperty(asEP_DISALLOW_VALUE_ASSIGN_FOR_REF_TYPE)) {
					// Verify that there is a default factory
					for (asUINT n = 0; n < subtype->GetFactoryCount(); n++) {
						asIScriptFunction* func = subtype->GetFactoryByIndex(n);
						if (func->GetParamCount() == 0) {
							// Found the default factory
							found = true;
							break;
						}
					}
				}

				if (!found) {
					// No default factory
					// TODO: Should format the message to give the name of the subtype for better understanding
					ti->GetEngine()->WriteMessage("array", 0, 0, asMSGTYPE_ERROR, "The subtype has no default factory");
					return false;
				}
			}

			// If the object type is not garbage collected then the array also doesn't need to be
			if (!(flags & asOBJ_GC))
				dontGarbageCollect = true;
		} else if (!(typeId & asTYPEID_OBJHANDLE)) {
			// Arrays with primitives cannot form circular references,
			// thus there is no need to garbage collect them
			dontGarbageCollect = true;
		} else {
			assert(typeId & asTYPEID_OBJHANDLE);

			// It is not necessary to set the array as garbage collected for all handle types.
			// If it is possible to determine that the handle cannot refer to an object type
			// that can potentially form a circular reference with the array then it is not 
			// necessary to make the array garbage collected.
			asITypeInfo* subtype = ti->GetEngine()->GetTypeInfoById(typeId);
			asDWORD flags = subtype->GetFlags();
			if (!(flags & asOBJ_GC)) {
				if ((flags & asOBJ_SCRIPT_OBJECT)) {
					// Even if a script class is by itself not garbage collected, it is possible
					// that classes that derive from it may be, so it is not possible to know 
					// that no circular reference can occur.
					if ((flags & asOBJ_NOINHERIT)) {
						// A script class declared as final cannot be inherited from, thus
						// we can be certain that the object cannot be garbage collected.
						dontGarbageCollect = true;
					}
				} else {
					// For application registered classes we assume the application knows
					// what it is doing and don't mark the array as garbage collected unless
					// the type is also garbage collected.
					dontGarbageCollect = true;
				}
			}
		}

		// The type is ok
		return true;
	}

	void ImportEngineFromJson(const char* file, asIScriptEngine* engine) {
		FILE* fin = fopen(file, "rb");
		if (!fin) {
			return;
		}
		fseek(fin, 0, SEEK_END);
		uint32_t size = ftell(fin);
		fseek(fin, 0, SEEK_SET);
		char* buffer = (char*)malloc(size);
		if (!buffer) {
			return;
		}
		rapidjson::Document document;
		rapidjson::FileReadStream stream(fin, buffer, size);
		document.ParseStream(stream);
		fclose(fin);
		if (document.HasParseError()) {
			return;
		}
		int r = 0;
		if (document.HasMember("GlobalEnums")) {
			const Value& globalEnums = document["GlobalEnums"];
			for (const auto& e : globalEnums.GetArray()) {
				const char* name = e["Name"].GetString();
				r = engine->RegisterEnum(name);
				assert(r >= 0);
				const Value& values = e["Values"];
				for (auto& v : values.GetArray()) {
					r = engine->RegisterEnumValue(name, v["Name"].GetString(), v["Value"].GetInt());
					assert(r >= 0);
				}
			}
		}
		if (document.HasMember("GlobalTypeDefs")) {
			const Value& globalTypeDefs = document["GlobalTypeDefs"];
			for (const auto& typeDef : globalTypeDefs.GetArray()) {
				r = engine->RegisterTypedef(typeDef["Name"].GetString(), typeDef["Type"].GetString());
				assert(r >= 0);
			}
		}
		
		if (document.HasMember("GlobalTypes")) {
			const Value& globalTypes = document["GlobalTypes"];
			for (const auto& type : globalTypes.GetArray()) {
				int flags = type["Flags"].GetInt();
				const char* decl = type["Declaration"].GetString();
				if (flags & asOBJ_SCRIPT_OBJECT) {
					r = engine->RegisterInterface(decl);
				} else {
					r = engine->RegisterObjectType(decl, (flags & asOBJ_VALUE) ? 1 : 0, flags);
				}
				assert(r >= 0);
				for (auto& beh : type["Behaviors"].GetArray()) {
					asEBehaviours b = (asEBehaviours)beh["Type"].GetInt();
					if (b == asBEHAVE_TEMPLATE_CALLBACK) {
						r = engine->RegisterObjectBehaviour(decl, b, beh["Func"]["Declaration"].GetString(), asFUNCTION(ScriptArrayTemplateCallback), asCALL_CDECL);
					} else {
						r = engine->RegisterObjectBehaviour(decl, b, beh["Func"]["Declaration"].GetString(), asFUNCTION(0), asCALL_GENERIC);
					}
					assert(r >= 0);
				}
			}
			//Hopefully we will have registered the string type now
			if (document.HasMember("StringFactoryType")) {
				const Value& stringType = document["StringFactoryType"];
				dummyStringFactory = new StringFactory();
				r = engine->RegisterStringFactory(stringType.GetString(), dummyStringFactory);
				assert(r >= 0);
			}
			//Hopefully we will have registered the array type
			if (document.HasMember("DefaultArrayType")) {
				const Value& arrayType = document["DefaultArrayType"];
				r = engine->RegisterDefaultArrayType(arrayType.GetString());
				assert(r >= 0);
			}
			if (document.HasMember("GlobalFuncDefs")) {
				const Value& globalFuncDefs = document["GlobalFuncDefs"];
				for (const auto& funcDef : globalFuncDefs.GetArray()) {
					r = engine->RegisterFuncdef(funcDef["Declaration"].GetString());
					assert(r >= 0);
				}
			}
			for (const auto& type : globalTypes.GetArray()) {
				const char* name = type["Name"].GetString();
				const char* decl = type["Declaration"].GetString();
				int flags = type["Flags"].GetInt();
				asITypeInfo* asType = engine->GetTypeInfoByName(name);
				if (asType) {
					for (auto& prop : type["Properties"].GetArray()) {
						r = engine->RegisterObjectProperty(decl, prop["Declaration"].GetString(), asType->GetPropertyCount());
						assert(r >= 0);
					}
					for (auto& method : type["Methods"].GetArray()) {
						if (flags & asOBJ_SCRIPT_OBJECT) {
							r = engine->RegisterInterfaceMethod(decl, method["Declaration"].GetString());
							assert(r >= 0);
						} else {
							const char* methodDecl = method["Declaration"].GetString();
							r = engine->RegisterObjectMethod(decl, methodDecl, asFUNCTION(0), asCALL_GENERIC);
							assert(r >= 0);
						}
					}
				}
			}
		}
		

		if (document.HasMember("GlobalProperties")) {
			const Value& globalProps = document["GlobalProperties"];
			for (const auto& prop : globalProps.GetArray()) {
				engine->RegisterGlobalProperty(prop["Declaration"].GetString(), nullptr);
			}
		}
		
		if (document.HasMember("GlobalFunctions")) {
			const Value& globalFunctions = document["GlobalFunctions"];
			for (const auto& func : globalFunctions.GetArray()) {
				r = engine->RegisterGlobalFunction(func["Declaration"].GetString(), asFUNCTION(0), asCALL_GENERIC);
				assert(r >= 0);
			}
		}

		free(buffer);
	}
}


