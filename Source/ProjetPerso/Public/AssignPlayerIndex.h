// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

static int currentPlayerIndexToAssign = 0;

inline static int GetNextPlayerIndex() {
	return currentPlayerIndexToAssign++;
}
