#include <cassert>
#include <memory>
#include <initializer_list>

enum Color { R, B };

// 1. No red node has a red child.
// 2. Every path from root to empty node contains the same
// number of black nodes.

template<class T>
class RBTree
{
	struct Node
	{
		Node(Color c, 
			std::shared_ptr<const Node> const & lft, 
			T val, 
			std::shared_ptr<const Node> const & rgt)
			: _c(c), _lft(lft), _val(val), _rgt(rgt)
		{}
		Color _c;
		std::shared_ptr<const Node> _lft;
		T _val;
		std::shared_ptr<const Node> _rgt;
	};
	explicit RBTree(std::shared_ptr<const Node> const & node) : _root(node) {} 
	Color rootColor() const
	{
		assert (!isEmpty());
		return _root->_c;
	}
public:
	RBTree() {}
	RBTree(Color c, RBTree const & lft, T val, RBTree const & rgt)
		: _root(std::make_shared<const Node>(c, lft._root, val, rgt._root))
	{}
	RBTree(std::initializer_list<T> init)
	{
		RBTree t;
		for (T v : init)
		{
			t = t.insert(v);
		}
		_root = t._root;
	}
	bool isEmpty() const { return !_root; }
	T root() const
	{
		assert(!isEmpty());
		return _root->_val;
	}
	RBTree left() const
	{
		assert(!isEmpty());
		return RBTree(_root->_lft);
	}
	RBTree right() const
	{
		assert(!isEmpty());
		return RBTree(_root->_rgt);
	}
	bool member(T x) const
	{
		if (isEmpty())
			return false;
		T y = root();
		if (x < y)
			return left().member(x);
		else if (y < x)
			return right().member(x);
		else
			return true;
	}
	RBTree insert(T x) const
	{
		RBTree t = ins(x);
		return RBTree(B, t.left(), t.root(), t.right());
	}
	// 1. No red node has a red child.
	void assert1() const
	{
		if (!isEmpty())
		{
			auto lft = left();
			auto rgt = right();
			if (rootColor() == R) 
			{
				assert(lft.isEmpty() || lft.rootColor() == B);
				assert(rgt.isEmpty() || rgt.rootColor() == B);
			}
			lft.assert1();
			rgt.assert1();
		}
	}
	// 2. Every path from root to empty node contains the same
	// number of black nodes.
	int countB() const
	{
		if (isEmpty())
			return 0;
		int lft = left().countB();
		int rgt = right().countB();
		assert(lft == rgt);
		return (rootColor() == B)? 1 + lft: lft;
	}
private:
	RBTree ins(T x) const
	{
		assert1();
		if (isEmpty())
			return RBTree(R, RBTree(), x, RBTree());
		T y = root();
		Color c = rootColor();
		if (x < y)
			return balance(c, left().ins(x), y, right());
		else if (y < x)
			return balance(c, left(), y, right().ins(x));
		else
			return *this; // no duplicates
	}
	static RBTree balance(Color c, RBTree const & lft, T x, RBTree const & rgt)
	{
		if (c == B && lft.doubledLeft())
			return RBTree(R
						, lft.left().paint(B)
						, lft.root()
						, RBTree(B, lft.right(), x, rgt));
		else if (c == B && lft.doubledRight())
			return RBTree(R
						, RBTree(B, lft.left(), lft.root(), lft.right().left())
						, lft.right().root()
						, RBTree(B, lft.right().right(), x, rgt));
		else if (c == B && rgt.doubledLeft())
			return RBTree(R
						, RBTree(B, lft, x, rgt.left().left())
						, rgt.left().root()
						, RBTree(B, rgt.left().right(), rgt.root(), rgt.right()));
		else if (c == B && rgt.doubledRight())
			return RBTree(R
						, RBTree(B, lft, x, rgt.left())
						, rgt.root()
						, rgt.right().paint(B));
		else
			return RBTree(c, lft, x, rgt);
	}
	bool doubledLeft() const 
	{
		return !isEmpty()
			&& rootColor() == R
			&& !left().isEmpty()
			&& left().rootColor() == R;
	}
	bool doubledRight() const 
	{
		return !isEmpty()
			&& rootColor() == R
			&& !right().isEmpty()
			&& right().rootColor() == R;
	}
	RBTree paint(Color c) const
	{
		assert(!isEmpty());
		return RBTree(c, left(), root(), right());
	}
private:
	std::shared_ptr<const Node> _root;
};

template<class T, class F>
void forEach(RBTree<T> const & t, F f) {
	if (!t.isEmpty()) {
		forEach(t.left(), f);
		f(t.root());
		forEach(t.right(), f);
	}
}

template<class T, class Beg, class End>
RBTree<T> insert(RBTree<T> t, Beg it, End end)
{
    if (it == end)
        return t;
    T item = *it;
	auto t1 = insert(t, ++it, end);
	return t1.insert(item);
}


