// Generated by Haxe 4.3.1
#ifndef INCLUDED_algorithm_PriorityQueue
#define INCLUDED_algorithm_PriorityQueue

#ifndef HXCPP_H
#include <hxcpp.h>
#endif

#ifndef INCLUDED_haxe_ds_GenericStack_algorithm_Node
#include <haxe/ds/GenericStack_algorithm_Node.h>
#endif
HX_DECLARE_CLASS1(algorithm,Node)
HX_DECLARE_CLASS1(algorithm,PriorityQueue)
HX_DECLARE_CLASS2(haxe,ds,GenericStack_algorithm_Node)

namespace algorithm{


class HXCPP_CLASS_ATTRIBUTES PriorityQueue_obj : public  ::haxe::ds::GenericStack_algorithm_Node_obj
{
	public:
		typedef  ::haxe::ds::GenericStack_algorithm_Node_obj super;
		typedef PriorityQueue_obj OBJ_;
		PriorityQueue_obj();

	public:
		enum { _hx_ClassId = 0x49aa37de };

		void __construct();
		inline void *operator new(size_t inSize, bool inContainer=true,const char *inName="algorithm.PriorityQueue")
			{ return ::hx::Object::operator new(inSize,inContainer,inName); }
		inline void *operator new(size_t inSize, int extra)
			{ return ::hx::Object::operator new(inSize+extra,true,"algorithm.PriorityQueue"); }
		static ::hx::ObjectPtr< PriorityQueue_obj > __new();
		static ::hx::ObjectPtr< PriorityQueue_obj > __alloc(::hx::Ctx *_hx_ctx);
		static void * _hx_vtable;
		static Dynamic __CreateEmpty();
		static Dynamic __Create(::hx::DynamicArray inArgs);
		//~PriorityQueue_obj();

		HX_DO_RTTI_ALL;
		::hx::Val __Field(const ::String &inString, ::hx::PropertyAccess inCallProp);
		static void __register();
		bool _hx_isInstanceOf(int inClassId);
		::String __ToString() const { return HX_("PriorityQueue",6d,07,1c,de); }

		void addByF( ::algorithm::Node item);
		::Dynamic addByF_dyn();

		bool contains( ::algorithm::Node n);
		::Dynamic contains_dyn();

		 ::algorithm::Node popN();
		::Dynamic popN_dyn();

		void clear();
		::Dynamic clear_dyn();

};

} // end namespace algorithm

#endif /* INCLUDED_algorithm_PriorityQueue */ 
