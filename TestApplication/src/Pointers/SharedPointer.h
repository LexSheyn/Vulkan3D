#ifndef SHAREDPOINTER_H
#define SHAREDPOINTER_H

namespace t3d
{
	template <typename T>
	using Ref = std::shared_ptr<T>;
}

#endif // SHAREDPOINTER_H