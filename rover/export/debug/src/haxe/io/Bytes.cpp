// Generated by Haxe 4.3.1
#include <hxcpp.h>

#ifndef INCLUDED_haxe_io_Bytes
#include <haxe/io/Bytes.h>
#endif
#ifndef INCLUDED_haxe_io_Encoding
#include <haxe/io/Encoding.h>
#endif
#ifndef INCLUDED_haxe_io_Error
#include <haxe/io/Error.h>
#endif

HX_DEFINE_STACK_FRAME(_hx_pos_dd4549ff8fa56c0b_34_new,"haxe.io.Bytes","new",0x3938d57d,"haxe.io.Bytes.new","/usr/local/lib/haxe/std/haxe/io/Bytes.hx",34,0xd68ef8f0)
HX_LOCAL_STACK_FRAME(_hx_pos_dd4549ff8fa56c0b_415_getString,"haxe.io.Bytes","getString",0xa16beae4,"haxe.io.Bytes.getString","/usr/local/lib/haxe/std/haxe/io/Bytes.hx",415,0xd68ef8f0)
HX_LOCAL_STACK_FRAME(_hx_pos_dd4549ff8fa56c0b_516_toString,"haxe.io.Bytes","toString",0x0291226f,"haxe.io.Bytes.toString","/usr/local/lib/haxe/std/haxe/io/Bytes.hx",516,0xd68ef8f0)
HX_LOCAL_STACK_FRAME(_hx_pos_dd4549ff8fa56c0b_580_ofString,"haxe.io.Bytes","ofString",0x6e53bb0b,"haxe.io.Bytes.ofString","/usr/local/lib/haxe/std/haxe/io/Bytes.hx",580,0xd68ef8f0)
namespace haxe{
namespace io{

void Bytes_obj::__construct(int length,::Array< unsigned char > b){
            	HX_STACKFRAME(&_hx_pos_dd4549ff8fa56c0b_34_new)
HXLINE(  35)		this->length = length;
HXLINE(  36)		this->b = b;
            	}

Dynamic Bytes_obj::__CreateEmpty() { return new Bytes_obj; }

void *Bytes_obj::_hx_vtable = 0;

Dynamic Bytes_obj::__Create(::hx::DynamicArray inArgs)
{
	::hx::ObjectPtr< Bytes_obj > _hx_result = new Bytes_obj();
	_hx_result->__construct(inArgs[0],inArgs[1]);
	return _hx_result;
}

bool Bytes_obj::_hx_isInstanceOf(int inClassId) {
	return inClassId==(int)0x00000001 || inClassId==(int)0x195c64b7;
}

::String Bytes_obj::getString(int pos,int len, ::haxe::io::Encoding encoding){
            	HX_STACKFRAME(&_hx_pos_dd4549ff8fa56c0b_415_getString)
HXLINE( 416)		bool _hx_tmp = ::hx::IsNull( encoding );
HXLINE( 419)		bool _hx_tmp1;
HXDLIN( 419)		bool _hx_tmp2;
HXDLIN( 419)		if ((pos >= 0)) {
HXLINE( 419)			_hx_tmp2 = (len < 0);
            		}
            		else {
HXLINE( 419)			_hx_tmp2 = true;
            		}
HXDLIN( 419)		if (!(_hx_tmp2)) {
HXLINE( 419)			_hx_tmp1 = ((pos + len) > this->length);
            		}
            		else {
HXLINE( 419)			_hx_tmp1 = true;
            		}
HXDLIN( 419)		if (_hx_tmp1) {
HXLINE( 420)			HX_STACK_DO_THROW(::haxe::io::Error_obj::OutsideBounds_dyn());
            		}
HXLINE( 428)		::String result = HX_("",00,00,00,00);
HXLINE( 429)		 ::__hxcpp_string_of_bytes(this->b,result,pos,len);
HXLINE( 430)		return result;
            	}


HX_DEFINE_DYNAMIC_FUNC3(Bytes_obj,getString,return )

::String Bytes_obj::toString(){
            	HX_STACKFRAME(&_hx_pos_dd4549ff8fa56c0b_516_toString)
HXDLIN( 516)		return this->getString(0,this->length,null());
            	}


HX_DEFINE_DYNAMIC_FUNC0(Bytes_obj,toString,return )

 ::haxe::io::Bytes Bytes_obj::ofString(::String s, ::haxe::io::Encoding encoding){
            	HX_GC_STACKFRAME(&_hx_pos_dd4549ff8fa56c0b_580_ofString)
HXLINE( 591)		::Array< unsigned char > a = ::Array_obj< unsigned char >::__new();
HXLINE( 592)		 ::__hxcpp_bytes_of_string(a,s);
HXLINE( 593)		return  ::haxe::io::Bytes_obj::__alloc( HX_CTX ,a->length,a);
            	}


STATIC_HX_DEFINE_DYNAMIC_FUNC2(Bytes_obj,ofString,return )


::hx::ObjectPtr< Bytes_obj > Bytes_obj::__new(int length,::Array< unsigned char > b) {
	::hx::ObjectPtr< Bytes_obj > __this = new Bytes_obj();
	__this->__construct(length,b);
	return __this;
}

::hx::ObjectPtr< Bytes_obj > Bytes_obj::__alloc(::hx::Ctx *_hx_ctx,int length,::Array< unsigned char > b) {
	Bytes_obj *__this = (Bytes_obj*)(::hx::Ctx::alloc(_hx_ctx, sizeof(Bytes_obj), true, "haxe.io.Bytes"));
	*(void **)__this = Bytes_obj::_hx_vtable;
	__this->__construct(length,b);
	return __this;
}

Bytes_obj::Bytes_obj()
{
}

void Bytes_obj::__Mark(HX_MARK_PARAMS)
{
	HX_MARK_BEGIN_CLASS(Bytes);
	HX_MARK_MEMBER_NAME(length,"length");
	HX_MARK_MEMBER_NAME(b,"b");
	HX_MARK_END_CLASS();
}

void Bytes_obj::__Visit(HX_VISIT_PARAMS)
{
	HX_VISIT_MEMBER_NAME(length,"length");
	HX_VISIT_MEMBER_NAME(b,"b");
}

::hx::Val Bytes_obj::__Field(const ::String &inName,::hx::PropertyAccess inCallProp)
{
	switch(inName.length) {
	case 1:
		if (HX_FIELD_EQ(inName,"b") ) { return ::hx::Val( b ); }
		break;
	case 6:
		if (HX_FIELD_EQ(inName,"length") ) { return ::hx::Val( length ); }
		break;
	case 8:
		if (HX_FIELD_EQ(inName,"toString") ) { return ::hx::Val( toString_dyn() ); }
		break;
	case 9:
		if (HX_FIELD_EQ(inName,"getString") ) { return ::hx::Val( getString_dyn() ); }
	}
	return super::__Field(inName,inCallProp);
}

bool Bytes_obj::__GetStatic(const ::String &inName, Dynamic &outValue, ::hx::PropertyAccess inCallProp)
{
	switch(inName.length) {
	case 8:
		if (HX_FIELD_EQ(inName,"ofString") ) { outValue = ofString_dyn(); return true; }
	}
	return false;
}

::hx::Val Bytes_obj::__SetField(const ::String &inName,const ::hx::Val &inValue,::hx::PropertyAccess inCallProp)
{
	switch(inName.length) {
	case 1:
		if (HX_FIELD_EQ(inName,"b") ) { b=inValue.Cast< ::Array< unsigned char > >(); return inValue; }
		break;
	case 6:
		if (HX_FIELD_EQ(inName,"length") ) { length=inValue.Cast< int >(); return inValue; }
	}
	return super::__SetField(inName,inValue,inCallProp);
}

void Bytes_obj::__GetFields(Array< ::String> &outFields)
{
	outFields->push(HX_("length",e6,94,07,9f));
	outFields->push(HX_("b",62,00,00,00));
	super::__GetFields(outFields);
};

#ifdef HXCPP_SCRIPTABLE
static ::hx::StorageInfo Bytes_obj_sMemberStorageInfo[] = {
	{::hx::fsInt,(int)offsetof(Bytes_obj,length),HX_("length",e6,94,07,9f)},
	{::hx::fsObject /* ::Array< unsigned char > */ ,(int)offsetof(Bytes_obj,b),HX_("b",62,00,00,00)},
	{ ::hx::fsUnknown, 0, null()}
};
static ::hx::StaticInfo *Bytes_obj_sStaticStorageInfo = 0;
#endif

static ::String Bytes_obj_sMemberFields[] = {
	HX_("length",e6,94,07,9f),
	HX_("b",62,00,00,00),
	HX_("getString",07,b2,86,8d),
	HX_("toString",ac,d0,6e,38),
	::String(null()) };

::hx::Class Bytes_obj::__mClass;

static ::String Bytes_obj_sStaticFields[] = {
	HX_("ofString",48,69,31,a4),
	::String(null())
};

void Bytes_obj::__register()
{
	Bytes_obj _hx_dummy;
	Bytes_obj::_hx_vtable = *(void **)&_hx_dummy;
	::hx::Static(__mClass) = new ::hx::Class_obj();
	__mClass->mName = HX_("haxe.io.Bytes",0b,53,6f,9f);
	__mClass->mSuper = &super::__SGetClass();
	__mClass->mConstructEmpty = &__CreateEmpty;
	__mClass->mConstructArgs = &__Create;
	__mClass->mGetStaticField = &Bytes_obj::__GetStatic;
	__mClass->mSetStaticField = &::hx::Class_obj::SetNoStaticField;
	__mClass->mStatics = ::hx::Class_obj::dupFunctions(Bytes_obj_sStaticFields);
	__mClass->mMembers = ::hx::Class_obj::dupFunctions(Bytes_obj_sMemberFields);
	__mClass->mCanCast = ::hx::TCanCast< Bytes_obj >;
#ifdef HXCPP_SCRIPTABLE
	__mClass->mMemberStorageInfo = Bytes_obj_sMemberStorageInfo;
#endif
#ifdef HXCPP_SCRIPTABLE
	__mClass->mStaticStorageInfo = Bytes_obj_sStaticStorageInfo;
#endif
	::hx::_hx_RegisterClass(__mClass->mName, __mClass);
}

} // end namespace haxe
} // end namespace io