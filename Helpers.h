/*
    This file is part of swan2 project.

    Copyright (C) 2013 by Nick Sharmanzhinov 
    except134@gmail.com
*/

#pragma once

namespace swan2
{
	namespace lib
	{
		namespace core 
		{
			template<typename T>
			inline void SafeNull(T& value)
			{
				value=NULL;
			}

			template<typename T>
			inline void SafeDelete(T& value)
			{
				if(value!=NULL) 
					delete value;
				value=NULL;
			}

			template<typename T>
			inline void SafeFree(T& value)
			{
				if(value!=NULL)
					free(value);
				Value=NULL;
			}

			template<typename T>
			inline void SafeDeleteArray(T& array)
			{
				if(array!=NULL) 
					delete[] array;
				array=NULL;
			}

			template<typename T>
			inline void SafeRelease(T& value)
			{
				if(value!=NULL) 
					value->Release();
				value=NULL;
			}

			template<typename T>
			inline void SafeNullArray(T* array,size_t number_of_elements)
			{
				for(indexer i=0;i<number_of_elements;i++)
					array[i]=T(NULL);
			}

			template <class T>
			inline void SwapValue(T& value1,T& value2)
			{
				T temp(std::move(value1));
				value1=std::move(value2);
				value2=std::move(temp);
			}

			inline errno_t SafeFopen(FILE*& f,const char* n,const char* m)
			{
				errno_t err=0;
				if(!f)
					err=fopen_s(&f,n,m);
				return err;
			}

			inline void SafeFclose(FILE*& f)
			{
				if(f) {
					fclose(f);
					f=NULL;
				}
			}

			inline errno_t SafeFopen(FILE*& f,const std::string n,const std::string m)
			{
				return SafeFopen(f,n.c_str(),m.c_str());
			}

			struct SHandleCloser 
			{ 
				void operator()(HANDLE h) 
				{ 
					if(h) 
						CloseHandle(h); 
				} 
			};

//			typedef public std::unique_ptr<void,SHandleCloser> ScopedHandle;

			inline HANDLE SafeHandle(HANDLE h) 
			{ 
				return (h==INVALID_HANDLE_VALUE)?0:h; 
			}

			template<class T> 
			class ScopedObject
			{
			public:
				explicit ScopedObject(T* p=0) : pointer(p) {}

				~ScopedObject()
				{
					SafeRelease(pointer);
				}

				bool IsNull() const 
				{ 
					return (!pointer); 
				}

				T& operator*()	
				{ 
					return *pointer; 
				}

				T* operator->() 
				{ 
					return pointer; 
				}

				T** operator&() 
				{ 
					return &pointer; 
				}

				void Reset(T* p=0) 
				{ 
					if(pointer) { 
						pointer->Release(); 
					} 
					pointer=p; 
				}

				T* Get() const 
				{ 
					return pointer; 
				}

			private:
				ScopedObject(const ScopedObject&);
				ScopedObject& operator=(const ScopedObject&);

				T* pointer;
			};

			template<typename T> 
			class SingletonStatic
			{
			public:
				static T& Instance() 
				{
					static T instance; 
					return instance;
				}
			};

			class NonCopyable
			{
			protected:
				NonCopyable() {};
				virtual ~NonCopyable() {};

			private:
				NonCopyable(const NonCopyable&);
				NonCopyable& operator=(const NonCopyable&);
			};

/*
			class NonMoveable
			{
			protected:
				NonMoveable() {};
				virtual ~NonMoveable() {};

			private:
				NonMoveable(NonMoveable&&);
				NonMoveable& operator=(NonMoveable&&);
			};*/
		}
	}
}

