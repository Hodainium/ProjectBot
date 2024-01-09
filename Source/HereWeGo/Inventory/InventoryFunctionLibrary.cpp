// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryFunctionLibrary.h"

#include "Kismet/KismetMathLibrary.h"

int32 UInventoryFunctionLibrary::GetColumn(const int32 ArrayIndex, const int32 Rows)
{
	double Remainder;

	UKismetMathLibrary::FMod(ArrayIndex, Rows, Remainder);

	return UKismetMathLibrary::FTrunc(Remainder);
}

int32 UInventoryFunctionLibrary::GetRow(const int32 ArrayIndex, const int32 Rows)
{
	if (Rows <= 0) return -1;
	const double TempNum = ArrayIndex / Rows;
	return UKismetMathLibrary::FTrunc(TempNum);
}
