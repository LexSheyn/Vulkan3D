#ifndef UNIQUEPOINTER_H
#define UNIQUEPOINTER_H

namespace t3d
{
	template <typename T>
	using Scope = std::unique_ptr<T>;
}

#endif // UNIQUEPOINTER_H