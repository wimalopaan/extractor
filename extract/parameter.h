#pragma once
#include<string>
struct parameter_t
{
	bool skipEmpptyLines;
	bool skipBlockComments;
	bool skipSnippetDefs;
	bool skipCallouts;
	bool skipMultiSnippetDeliminter;
	bool skipHighlighting;
	bool enableEmptyLines;
	bool enableSnippetDefs;
	bool enableCallouts;
	bool enableHighlighting;
	bool includeOmitted;
	bool filtermode;
	bool lineNumbers;
	auto indent;
	std::string language;
	std::string astyleParamet;
	std::string output;
	std::string subdir;
};
