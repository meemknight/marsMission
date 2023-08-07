// Generated by Haxe 4.3.1
#ifndef INCLUDED_haxe_ds_GenericStack_algorithm_Node
#define INCLUDED_haxe_ds_GenericStack_algorithm_Node

#ifndef HXCPP_H
#include <hxcpp.h>
#endif

HX_DECLARE_CLASS1(algorithm,Node)
HX_DECLARE_CLASS2(haxe,ds,GenericCell_algorithm_Node)
HX_DECLARE_CLASS2(haxe,ds,GenericStack_algorithm_Node)

namespace haxe{
namespace ds{


class HXCPP_CLASS_ATTRIBUTES GenericStack_algorithm_Node_obj : public ::hx::Object
{
	public:
		typedef ::hx::Object super;
		typedef GenericStack_algorithm_Node_obj OBJ_;
		GenericStack_algorithm_Node_obj();

	public:
		enum { _hx_ClassId = 0x0c965727 };

		void __construct();
		inline void *operator new(size_t inSize, bool inContainer=true,const char *inName="haxe.ds.GenericStack_algorithm_Node")
			{ return ::hx::Object::operator new(inSize,inContainer,inName); }
		inline void *operator new(size_t inSize, int extra)
			{ return ::hx::Object::operator new(inSize+extra,true,"haxe.ds.GenericStack_algorithm_Node"); }
		static ::hx::ObjectPtr< GenericStack_algorithm_Node_obj > __new();
		static ::hx::ObjectPtr< GenericStack_algorithm_Node_obj > __alloc(::hx::Ctx *_hx_ctx);
		static void * _hx_vtable;
		static Dynamic __CreateEmpty();
		static Dynamic __Create(::hx::DynamicArray inArgs);
		//~GenericStack_algorithm_Node_obj();

		HX_DO_RTTI_ALL;
		::hx::Val __Field(const ::String &inString, ::hx::PropertyAccess inCallProp);
		::hx::Val __SetField(const ::String &inString,const ::hx::Val &inValue, ::hx::PropertyAccess inCallProp);
		void __GetFields(Array< ::String> &outFields);
		static void __register();
		void __Mark(HX_MARK_PARAMS);
		void __Visit(HX_VISIT_PARAMS);
		bool _hx_isInstanceOf(int inClassId);
		::String __ToString() const { return HX_("GenericStack_algorithm_Node",20,f1,79,d8); }

		 ::haxe::ds::GenericCell_algorithm_Node head;
		void add( ::algorithm::Node item);
		::Dynamic add_dyn();

		 ::algorithm::Node first();
		::Dynamic first_dyn();

		 ::algorithm::Node pop();
		::Dynamic pop_dyn();

		bool isEmpty();
		::Dynamic isEmpty_dyn();

		bool remove( ::algorithm::Node v);
		::Dynamic remove_dyn();

		 ::Dynamic iterator();
		::Dynamic iterator_dyn();

		virtual ::String toString();
		::Dynamic toString_dyn();

};

} // end namespace haxe
} // end namespace ds

#endif /* INCLUDED_haxe_ds_GenericStack_algorithm_Node */ 
