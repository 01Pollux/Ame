#ifndef AME_CORE_H
#define AME_CORE_H

#ifdef AME_SHADER_COMPILER_SPIRV

#define AME_RESOURCE(Type, Name, Register, BindingIndex, SetIndex) \
		Type Name : register( Register##BindingIndex, space##SetIndex )

#define AME_PUSH_CONSTANT(Type, Name, BindingIndex)
		[[vk::push_constant]] Type Name

#else

#define AME_RESOURCE(Type, Name, Register, BindingIndex, SetIndex) \
		Type Name : register( Register##BindingIndex, space##SetIndex )

#define AME_PUSH_CONSTANT(Type, Name, BindingIndex) \
		ConstantBuffer<Type> Name : register(b##BindingIndex, space0)

#endif

#endif // AME_CORE_SHADER_H