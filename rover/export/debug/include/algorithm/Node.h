// Generated by Haxe 4.3.1
#ifndef INCLUDED_algorithm_Node
#define INCLUDED_algorithm_Node

#ifndef HXCPP_H
#include <hxcpp.h>
#endif

HX_DECLARE_STACK_FRAME(_hx_pos_830657d472c1488f_13_new)
HX_DECLARE_CLASS1(algorithm,Node)

namespace algorithm{


class HXCPP_CLASS_ATTRIBUTES Node_obj : public ::hx::Object
{
	public:
		typedef ::hx::Object super;
		typedef Node_obj OBJ_;
		Node_obj();

	public:
		enum { _hx_ClassId = 0x121cc525 };

		void __construct(int x,int y);
		inline void *operator new(size_t inSize, bool inContainer=true,const char *inName="algorithm.Node")
			{ return ::hx::Object::operator new(inSize,inContainer,inName); }
		inline void *operator new(size_t inSize, int extra)
			{ return ::hx::Object::operator new(inSize+extra,true,"algorithm.Node"); }

		inline static ::hx::ObjectPtr< Node_obj > __new(int x,int y) {
			::hx::ObjectPtr< Node_obj > __this = new Node_obj();
			__this->__construct(x,y);
			return __this;
		}

		inline static ::hx::ObjectPtr< Node_obj > __alloc(::hx::Ctx *_hx_ctx,int x,int y) {
			Node_obj *__this = (Node_obj*)(::hx::Ctx::alloc(_hx_ctx, sizeof(Node_obj), true, "algorithm.Node"));
			*(void **)__this = Node_obj::_hx_vtable;
{
            	HX_STACKFRAME(&_hx_pos_830657d472c1488f_13_new)
HXLINE(  14)		( ( ::algorithm::Node)(__this) )->x = x;
HXLINE(  15)		( ( ::algorithm::Node)(__this) )->y = y;
HXLINE(  16)		( ( ::algorithm::Node)(__this) )->g = 0;
HXLINE(  17)		( ( ::algorithm::Node)(__this) )->h = 0;
HXLINE(  18)		( ( ::algorithm::Node)(__this) )->f = 0;
HXLINE(  19)		( ( ::algorithm::Node)(__this) )->direction = HX_("",00,00,00,00);
            	}
		
			return __this;
		}

		static void * _hx_vtable;
		static Dynamic __CreateEmpty();
		static Dynamic __Create(::hx::DynamicArray inArgs);
		//~Node_obj();

		HX_DO_RTTI_ALL;
		::hx::Val __Field(const ::String &inString, ::hx::PropertyAccess inCallProp);
		::hx::Val __SetField(const ::String &inString,const ::hx::Val &inValue, ::hx::PropertyAccess inCallProp);
		void __GetFields(Array< ::String> &outFields);
		static void __register();
		void __Mark(HX_MARK_PARAMS);
		void __Visit(HX_VISIT_PARAMS);
		bool _hx_isInstanceOf(int inClassId);
		::String __ToString() const { return HX_("Node",22,36,e3,33); }

		int g;
		int h;
		int f;
		int x;
		int y;
		 ::algorithm::Node parent;
		::String direction;
		::String _hx_char;
};

} // end namespace algorithm

#endif /* INCLUDED_algorithm_Node */ 
