// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIInfo.h"

/**
 * 
 */
class MOTE_API CMonsterDataManager
{
public:
	CMonsterDataManager();
	~CMonsterDataManager();

public:
	bool Init();

	const FMonsterAnimData* FindMonsterAnim(const FName& Key);
	const FMonsterInfoTable* FindMonsterInfo(const FName& Key);

private:
	UDataTable* mMonsterAnimTable = nullptr;
	UDataTable* mMonsterInfoTable = nullptr;

private:
	static CMonsterDataManager* mInst;

public:
	static CMonsterDataManager* GetInst()
	{
		if (!mInst)
			mInst = new CMonsterDataManager;
		return mInst;
	}

	static void DestroyInst()
	{
		if (mInst)
		{
			delete mInst;
			mInst = nullptr;
		}
	}
};
