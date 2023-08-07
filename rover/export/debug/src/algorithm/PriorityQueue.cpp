// Generated by Haxe 4.3.1
#include <hxcpp.h>

#ifndef INCLUDED_algorithm_Node
#include <algorithm/Node.h>
#endif
#ifndef INCLUDED_algorithm_PriorityQueue
#include <algorithm/PriorityQueue.h>
#endif
#ifndef INCLUDED_haxe_ds_GenericCell_algorithm_Node
#include <haxe/ds/GenericCell_algorithm_Node.h>
#endif
#ifndef INCLUDED_haxe_ds_GenericStack_algorithm_Node
#include <haxe/ds/GenericStack_algorithm_Node.h>
#endif

HX_DEFINE_STACK_FRAME(_hx_pos_5e954be312f7dcde_7_new,"algorithm.PriorityQueue","new",0xfc3ec320,"algorithm.PriorityQueue.new","algorithm/PriorityQueue.hx",7,0x6bf9d251)
HX_LOCAL_STACK_FRAME(_hx_pos_5e954be312f7dcde_10_addByF,"algorithm.PriorityQueue","addByF",0xe5dd1c2e,"algorithm.PriorityQueue.addByF","algorithm/PriorityQueue.hx",10,0x6bf9d251)
HX_LOCAL_STACK_FRAME(_hx_pos_5e954be312f7dcde_35_contains,"algorithm.PriorityQueue","contains",0xe2286f5f,"algorithm.PriorityQueue.contains","algorithm/PriorityQueue.hx",35,0x6bf9d251)
HX_LOCAL_STACK_FRAME(_hx_pos_5e954be312f7dcde_43_popN,"algorithm.PriorityQueue","popN",0xbc05f6dd,"algorithm.PriorityQueue.popN","algorithm/PriorityQueue.hx",43,0x6bf9d251)
HX_LOCAL_STACK_FRAME(_hx_pos_5e954be312f7dcde_69_clear,"algorithm.PriorityQueue","clear",0x4af8944d,"algorithm.PriorityQueue.clear","algorithm/PriorityQueue.hx",69,0x6bf9d251)
namespace algorithm{

void PriorityQueue_obj::__construct(){
            	HX_STACKFRAME(&_hx_pos_5e954be312f7dcde_7_new)
HXDLIN(   7)		super::__construct();
            	}

Dynamic PriorityQueue_obj::__CreateEmpty() { return new PriorityQueue_obj; }

void *PriorityQueue_obj::_hx_vtable = 0;

Dynamic PriorityQueue_obj::__Create(::hx::DynamicArray inArgs)
{
	::hx::ObjectPtr< PriorityQueue_obj > _hx_result = new PriorityQueue_obj();
	_hx_result->__construct();
	return _hx_result;
}

bool PriorityQueue_obj::_hx_isInstanceOf(int inClassId) {
	if (inClassId<=(int)0x0c965727) {
		return inClassId==(int)0x00000001 || inClassId==(int)0x0c965727;
	} else {
		return inClassId==(int)0x49aa37de;
	}
}

void PriorityQueue_obj::addByF( ::algorithm::Node item){
            	HX_GC_STACKFRAME(&_hx_pos_5e954be312f7dcde_10_addByF)
HXLINE(  11)		if (::hx::IsNull( this->head )) {
HXLINE(  13)			this->head =  ::haxe::ds::GenericCell_algorithm_Node_obj::__alloc( HX_CTX ,item,this->head);
HXLINE(  14)			return;
            		}
HXLINE(  17)		 ::haxe::ds::GenericCell_algorithm_Node previous = null();
HXLINE(  18)		 ::haxe::ds::GenericCell_algorithm_Node current = this->head;
HXLINE(  21)		while(true){
HXLINE(  21)			bool _hx_tmp;
HXDLIN(  21)			if (::hx::IsNotNull( current )) {
HXLINE(  21)				_hx_tmp = (item->f > current->elt->f);
            			}
            			else {
HXLINE(  21)				_hx_tmp = false;
            			}
HXDLIN(  21)			if (!(_hx_tmp)) {
HXLINE(  21)				goto _hx_goto_1;
            			}
HXLINE(  22)			previous = current;
HXLINE(  23)			current = current->next;
            		}
            		_hx_goto_1:;
HXLINE(  27)		if (::hx::IsNull( previous )) {
HXLINE(  28)			this->head =  ::haxe::ds::GenericCell_algorithm_Node_obj::__alloc( HX_CTX ,item,this->head);
            		}
            		else {
HXLINE(  30)			 ::haxe::ds::GenericCell_algorithm_Node newNode =  ::haxe::ds::GenericCell_algorithm_Node_obj::__alloc( HX_CTX ,item,current);
HXLINE(  31)			previous->next = newNode;
            		}
            	}


HX_DEFINE_DYNAMIC_FUNC1(PriorityQueue_obj,addByF,(void))

bool PriorityQueue_obj::contains( ::algorithm::Node n){
            	HX_STACKFRAME(&_hx_pos_5e954be312f7dcde_35_contains)
HXLINE(  36)		{
HXLINE(  36)			 ::Dynamic node = this->iterator();
HXDLIN(  36)			while(( (bool)(node->__Field(HX_("hasNext",6d,a5,46,18),::hx::paccDynamic)()) )){
HXLINE(  36)				 ::algorithm::Node node1 = ( ( ::algorithm::Node)(node->__Field(HX_("next",f3,84,02,49),::hx::paccDynamic)()) );
HXLINE(  37)				if (::hx::IsInstanceEq( node1,n )) {
HXLINE(  37)					return true;
            				}
            			}
            		}
HXLINE(  40)		return false;
            	}


HX_DEFINE_DYNAMIC_FUNC1(PriorityQueue_obj,contains,return )

 ::algorithm::Node PriorityQueue_obj::popN(){
            	HX_STACKFRAME(&_hx_pos_5e954be312f7dcde_43_popN)
HXLINE(  44)		 ::algorithm::Node lowestFNode = this->head->elt;
HXLINE(  45)		int lowestF = (lowestFNode->g + lowestFNode->h);
HXLINE(  46)		 ::haxe::ds::GenericCell_algorithm_Node current = this->head;
HXLINE(  47)		 ::haxe::ds::GenericCell_algorithm_Node previous = null();
HXLINE(  49)		while(::hx::IsNotNull( current )){
HXLINE(  50)			int f = (current->elt->g + current->elt->h);
HXLINE(  51)			if ((f < lowestF)) {
HXLINE(  52)				lowestF = f;
HXLINE(  53)				lowestFNode = current->elt;
HXLINE(  54)				previous = current;
            			}
HXLINE(  56)			current = current->next;
            		}
HXLINE(  59)		if (::hx::IsNull( previous )) {
HXLINE(  60)			this->head = this->head->next;
            		}
            		else {
HXLINE(  62)			previous->next = previous->next->next;
            		}
HXLINE(  65)		return lowestFNode;
            	}


HX_DEFINE_DYNAMIC_FUNC0(PriorityQueue_obj,popN,return )

void PriorityQueue_obj::clear(){
            	HX_STACKFRAME(&_hx_pos_5e954be312f7dcde_69_clear)
HXDLIN(  69)		 ::Dynamic node = this->iterator();
HXDLIN(  69)		while(( (bool)(node->__Field(HX_("hasNext",6d,a5,46,18),::hx::paccDynamic)()) )){
HXDLIN(  69)			 ::algorithm::Node node1 = ( ( ::algorithm::Node)(node->__Field(HX_("next",f3,84,02,49),::hx::paccDynamic)()) );
HXLINE(  70)			this->remove(node1);
            		}
            	}


HX_DEFINE_DYNAMIC_FUNC0(PriorityQueue_obj,clear,(void))


::hx::ObjectPtr< PriorityQueue_obj > PriorityQueue_obj::__new() {
	::hx::ObjectPtr< PriorityQueue_obj > __this = new PriorityQueue_obj();
	__this->__construct();
	return __this;
}

::hx::ObjectPtr< PriorityQueue_obj > PriorityQueue_obj::__alloc(::hx::Ctx *_hx_ctx) {
	PriorityQueue_obj *__this = (PriorityQueue_obj*)(::hx::Ctx::alloc(_hx_ctx, sizeof(PriorityQueue_obj), true, "algorithm.PriorityQueue"));
	*(void **)__this = PriorityQueue_obj::_hx_vtable;
	__this->__construct();
	return __this;
}

PriorityQueue_obj::PriorityQueue_obj()
{
}

::hx::Val PriorityQueue_obj::__Field(const ::String &inName,::hx::PropertyAccess inCallProp)
{
	switch(inName.length) {
	case 4:
		if (HX_FIELD_EQ(inName,"popN") ) { return ::hx::Val( popN_dyn() ); }
		break;
	case 5:
		if (HX_FIELD_EQ(inName,"clear") ) { return ::hx::Val( clear_dyn() ); }
		break;
	case 6:
		if (HX_FIELD_EQ(inName,"addByF") ) { return ::hx::Val( addByF_dyn() ); }
		break;
	case 8:
		if (HX_FIELD_EQ(inName,"contains") ) { return ::hx::Val( contains_dyn() ); }
	}
	return super::__Field(inName,inCallProp);
}

#ifdef HXCPP_SCRIPTABLE
static ::hx::StorageInfo *PriorityQueue_obj_sMemberStorageInfo = 0;
static ::hx::StaticInfo *PriorityQueue_obj_sStaticStorageInfo = 0;
#endif

static ::String PriorityQueue_obj_sMemberFields[] = {
	HX_("addByF",ee,d6,fb,9e),
	HX_("contains",1f,5a,7b,2c),
	HX_("popN",9d,81,5c,4a),
	HX_("clear",8d,71,5b,48),
	::String(null()) };

::hx::Class PriorityQueue_obj::__mClass;

void PriorityQueue_obj::__register()
{
	PriorityQueue_obj _hx_dummy;
	PriorityQueue_obj::_hx_vtable = *(void **)&_hx_dummy;
	::hx::Static(__mClass) = new ::hx::Class_obj();
	__mClass->mName = HX_("algorithm.PriorityQueue",2e,f3,1e,1b);
	__mClass->mSuper = &super::__SGetClass();
	__mClass->mConstructEmpty = &__CreateEmpty;
	__mClass->mConstructArgs = &__Create;
	__mClass->mGetStaticField = &::hx::Class_obj::GetNoStaticField;
	__mClass->mSetStaticField = &::hx::Class_obj::SetNoStaticField;
	__mClass->mStatics = ::hx::Class_obj::dupFunctions(0 /* sStaticFields */);
	__mClass->mMembers = ::hx::Class_obj::dupFunctions(PriorityQueue_obj_sMemberFields);
	__mClass->mCanCast = ::hx::TCanCast< PriorityQueue_obj >;
#ifdef HXCPP_SCRIPTABLE
	__mClass->mMemberStorageInfo = PriorityQueue_obj_sMemberStorageInfo;
#endif
#ifdef HXCPP_SCRIPTABLE
	__mClass->mStaticStorageInfo = PriorityQueue_obj_sStaticStorageInfo;
#endif
	::hx::_hx_RegisterClass(__mClass->mName, __mClass);
}

} // end namespace algorithm
