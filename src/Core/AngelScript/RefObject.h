#pragma once

#define REF_COUNTER public: void Increment(){ m_RefCounter++;};\
					public: void Release(){m_RefCounter--; if(m_RefCounter == 0) delete this;};\
					private: int m_RefCounter = 1;