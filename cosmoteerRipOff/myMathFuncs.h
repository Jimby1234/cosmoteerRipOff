#pragma once

int roundToNearest(int number, int roundTo)
{
	if (number >= 0)
	{
		return (number + roundTo / 2) / roundTo * roundTo;
	}
	else
	{
		return (number - roundTo / 2) / roundTo * roundTo;
	}
}