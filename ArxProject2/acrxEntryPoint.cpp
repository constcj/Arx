// (C) Copyright 2002-2012 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include <fstream>
#include <direct.h>
#include <atlconv.h>
#include <iostream>

#include <cassert>
#include <string>

#include <vector>

#include <regex>

#include<windows.h>
#include<cstdio>
//-----------------------------------------------------------------------------
#define szRDS _RXST("")
using namespace std;

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CArxProject2App : public AcRxArxApp {

public:
	CArxProject2App () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;

		// TODO: Add your initialization code here

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}

	// The ACED_ARXCOMMAND_ENTRY_AUTO macro can be applied to any static member 
	// function of the CArxProject2App class.
	// The function should take no arguments and return nothing.
	//
	// NOTE: ACED_ARXCOMMAND_ENTRY_AUTO has overloads where you can provide resourceid and
	// have arguments to define context and command mechanism.

	// ACED_ARXCOMMAND_ENTRY_AUTO(classname, group, globCmd, locCmd, cmdFlags, UIContext)
	// ACED_ARXCOMMAND_ENTRYBYID_AUTO(classname, group, globCmd, locCmdId, cmdFlags, UIContext)
	// only differs that it creates a localized name using a string in the resource file
	//   locCmdId - resource ID for localized command

	// Modal Command with localized name
	// ACED_ARXCOMMAND_ENTRY_AUTO(CArxProject2App, MyGroup, MyCommand, MyCommandLocal, ACRX_CMD_MODAL)
	static void MyGroupMyCommand () {

		wchar_t s[MAX_PATH];

		GetModuleFileName(GetModuleHandle(_T("ArxProject2.arx")),s,MAX_PATH);

		char* bf=ConvertToChar(s);
		string filename=bf;
		delete [] bf;

		string dir=filename.substr(0,filename.rfind('\\'));
		ifstream infile((dir+"\\find.txt").data());
		assert(infile.is_open());
		string c;
		vector<string> elm;
		while (getline(infile,c))
		{
			elm.push_back(c);
		}
		infile.close();

		ifstream inregex((dir+"\\regex.txt").data());
		assert(inregex.is_open());
		string reg;
		getline(inregex,reg);
		inregex.close();

		AcDbBlockTable *pBlkTb=nullptr;
		acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pBlkTb,kForRead);
		AcDbBlockTableRecord *pBlkRcd=nullptr;
		pBlkTb->getAt(ACDB_MODEL_SPACE,pBlkRcd,kForWrite);
		pBlkTb->close();
		AcDbBlockTableRecordIterator *pBlkTbRcdItr=nullptr;
		pBlkRcd->newIterator(pBlkTbRcdItr);
		AcDbEntity *pEnt=nullptr;	
		AcGePoint3d ptStart(0,0,0);

		for (pBlkTbRcdItr->start();!pBlkTbRcdItr->done();pBlkTbRcdItr->step())
		{
			pBlkTbRcdItr->getEntity(pEnt,kForRead);
			char* bs=ConvertToChar(pEnt->isA()->name());
			if(strcmp("AcDbText",bs)==0) {  
				AcDbText *eText=(AcDbText *)pEnt;
				string textcontent=ConvertToChar(eText->textString());

				if (!regex_search(textcontent,regex("^[TDMLJQ]-")))
				{
					string::const_iterator iterstart=textcontent.begin();
					string::const_iterator iterend=textcontent.end();

					smatch result;

					if (regex_search(iterstart,iterend,result,regex(reg)))
					{
						string temp=result[0];

						if (count(elm.begin(),elm.end(),temp))
						{						
							AcDbLine *pLine=new AcDbLine(ptStart,eText->position());
							pBlkRcd->appendAcDbEntity(pLine);
							pLine->close();
						}
					}

				}
				eText->close();
			}

			pEnt->close();

		}

		pBlkRcd->close();
		delete pBlkTbRcdItr;
		//acutPrintf(
		// Put your command code here
	}

	//wchar_t*£¬ACHAR*×ªchar*
	static inline	char * ConvertToChar(LPCWCH s)
	{
		int len=WideCharToMultiByte(CP_ACP,0,s,-1,NULL,0,NULL,NULL);
		char* bs=new char[len];
		WideCharToMultiByte(CP_ACP,0,s,-1,bs,len,NULL,NULL);
		return bs;
	}


	// Modal Command with pickfirst selection
	// ACED_ARXCOMMAND_ENTRY_AUTO(CArxProject2App, MyGroup, MyPickFirst, MyPickFirstLocal, ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET)
	static void MyGroupMyPickFirst () {
		ads_name result ;
		int iRet =acedSSGet (ACRX_T("_I"), NULL, NULL, NULL, result) ;
		if ( iRet == RTNORM )
		{
			// There are selected entities
			// Put your command using pickfirst set code here
		}
		else
		{
			// There are no selected entities
			// Put your command code here
		}
	}

	// Application Session Command with localized name
	// ACED_ARXCOMMAND_ENTRY_AUTO(CArxProject2App, MyGroup, MySessionCmd, MySessionCmdLocal, ACRX_CMD_MODAL | ACRX_CMD_SESSION)
	static void MyGroupMySessionCmd () {
		// Put your command code here
	}

	// The ACED_ADSFUNCTION_ENTRY_AUTO / ACED_ADSCOMMAND_ENTRY_AUTO macros can be applied to any static member 
	// function of the CArxProject2App class.
	// The function may or may not take arguments and have to return RTNORM, RTERROR, RTCAN, RTFAIL, RTREJ to AutoCAD, but use
	// acedRetNil, acedRetT, acedRetVoid, acedRetInt, acedRetReal, acedRetStr, acedRetPoint, acedRetName, acedRetList, acedRetVal to return
	// a value to the Lisp interpreter.
	//
	// NOTE: ACED_ADSFUNCTION_ENTRY_AUTO / ACED_ADSCOMMAND_ENTRY_AUTO has overloads where you can provide resourceid.

	//- ACED_ADSFUNCTION_ENTRY_AUTO(classname, name, regFunc) - this example
	//- ACED_ADSSYMBOL_ENTRYBYID_AUTO(classname, name, nameId, regFunc) - only differs that it creates a localized name using a string in the resource file
	//- ACED_ADSCOMMAND_ENTRY_AUTO(classname, name, regFunc) - a Lisp command (prefix C:)
	//- ACED_ADSCOMMAND_ENTRYBYID_AUTO(classname, name, nameId, regFunc) - only differs that it creates a localized name using a string in the resource file

	// Lisp Function is similar to ARX Command but it creates a lisp 
	// callable function. Many return types are supported not just string
	// or integer.
	// ACED_ADSFUNCTION_ENTRY_AUTO(CArxProject2App, MyLispFunction, false)
	static int ads_MyLispFunction () {
		//struct resbuf *args =acedGetArgs () ;

		// Put your command code here

		//acutRelRb (args) ;

		// Return a value to the AutoCAD Lisp Interpreter
		// acedRetNil, acedRetT, acedRetVoid, acedRetInt, acedRetReal, acedRetStr, acedRetPoint, acedRetName, acedRetList, acedRetVal

		return (RTNORM) ;
	}

} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CArxProject2App)

	ACED_ARXCOMMAND_ENTRY_AUTO(CArxProject2App, MyGroup, MyCommand, fido, ACRX_CMD_MODAL, NULL)
	//ACED_ARXCOMMAND_ENTRY_AUTO(CArxProject2App, MyGroup, MyPickFirst, MyPickFirstLocal, ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, NULL)
	//ACED_ARXCOMMAND_ENTRY_AUTO(CArxProject2App, MyGroup, MySessionCmd, MySessionCmdLocal, ACRX_CMD_MODAL | ACRX_CMD_SESSION, NULL)
	//ACED_ADSSYMBOL_ENTRY_AUTO(CArxProject2App, MyLispFunction, false)

