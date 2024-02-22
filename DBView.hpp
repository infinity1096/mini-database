#ifndef DBView_hpp
#define DBView_hpp

#include "View.hpp"
#include "FolderReader.hpp"

namespace ECE141 {
	class DBView : public View {
	public:
		DBView(const char *aPath, const char *anExtension=".db")
			{}
		virtual bool    show(std::ostream& aStream) = 0;
	};
}

#endif

