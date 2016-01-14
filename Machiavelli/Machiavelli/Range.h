#pragma once

struct Range
{
	int first;
	int last;

	Range(int first, int last) {
		this->first = first;
		this->last = last;
	}
};