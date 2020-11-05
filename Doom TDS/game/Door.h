#pragma once
#include "Model.h"

class CDoor : public CModel
{
	private:

	CVector openPosition, closePosition;
	
	public:
	
	void SetPositions(CVector start, CVector end);
	void Open();
	void Close();
	
};
