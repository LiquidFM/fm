#ifndef AVLTREE_H_
#define AVLTREE_H_

#define AVL_SEARCH_TREE_DONT_OPTIMIZE // ��������� �������������� �����������
//#define CONSOLE_AVLTREE
//#define DEBUG_AVLTREE
#if defined CONSOLE_AVLTREE
#	include <iostream>
#endif


/*****************
 *** Interface ***
 *****************/
template <typename AvlKey, typename SizeType>
class AvlTree
{
public:
	typedef AvlKey   value_type;
	typedef SizeType size_type;

	//----------------------------------------------------------
	// AvlNode
	//
	// ��������: ������� ����� ������ �����������,
	// ��� ��� ������� search_routine ����������
	// �������������� �����������. ��������� �������
	// ����� �������� � �������� ������ ���������.
	// ���������� ���� � ����� ���������, � �� � ������
	// (����� ������������ ������������)
	//
	// offsetof()
	//----------------------------------------------------------
	struct AvlNode
	{
		struct Links {AvlNode *left_node; AvlNode *right_node; AvlNode *parent_node;} links;
		value_type value;
		enum Balanced {BALANCED = 0, LEFT_IS_HEAVY = -1, RIGHT_IS_HEAVY = 1} balance;

		//----------------------------------------------------------
		AvlNode(const value_type &value, AvlNode *parent_node) :
			value(value),
			balance(BALANCED)
		{
			links.left_node = 0;
			links.right_node = 0;
			links.parent_node = parent_node;
		}
		virtual ~AvlNode() {}

		//----------------------------------------------------------
		static void destroy_parent_to_child_link(AvlNode *child);

		//----------------------------------------------------------
		void change_left(
			AvlNode *child,
			AvlNode *&out_child_old_parent, // ������ �������� child
			AvlNode *&out_parent_left_node // ������ �������� ���� parent
		);

		//----------------------------------------------------------
		void change_right(
			AvlNode *child,
			AvlNode *&out_child_old_parent, // ������ �������� child
			AvlNode *&out_parent_right_node // ������ �������� ���� parent
		);

		//----------------------------------------------------------
		// � ���������� ������ ������� new_node
		// ������ ������� node_to_replace � �������� ������.
		//----------------------------------------------------------
		static void replace_node(
			AvlNode *node_to_replace,
			AvlNode *new_node,
			AvlNode *&root,
			// �������� ��������
			Links &new_node_old_links
		);

		//----------------------------------------------------------
		static void rebalance_left_grew(
			AvlNode *node_to_balance,
			AvlNode *&root
		);

		//----------------------------------------------------------
		static void rebalance_right_grew(
			AvlNode *node_to_balance,
			AvlNode *&root
		);

		//----------------------------------------------------------
		static void rebalance_grew_routine(
			typename AvlNode::Balanced grew,
			AvlNode *node_to_balance,
			AvlNode *&root
		);

		//----------------------------------------------------------
		static bool rebalance_left_shrunk(
			AvlNode *&node_to_balance,
			AvlNode *&root
		);

		//----------------------------------------------------------
		static bool rebalance_right_shrunk(
			AvlNode *&node_to_balance,
			AvlNode *&root
		);

		//----------------------------------------------------------
		static bool rebalance_shrunk_routine(
			typename AvlNode::Balanced shrunk,
			AvlNode *&node_to_balance,
			AvlNode *&root
		);

		//----------------------------------------------------------
		// ���������������� ��� ���������� ����
		//----------------------------------------------------------
		static void rebalance_grew(
			AvlNode *this_node,
			AvlNode *&root );

		//----------------------------------------------------------
		// ���������������� ��� �������� ����
		//----------------------------------------------------------
		static void rebalance_shrunk(
			AvlNode *this_node,
			value_type previous_node_value,
			AvlNode *&root
		);
	};

	//----------------------------------------------------------
	AvlTree() :
		tree_root(0),
		node_counter(0)
	{}

	//----------------------------------------------------------
	virtual ~AvlTree()
	{
		destroy_subtree(tree_root);
	}

	//----------------------------------------------------------
	inline AvlNode *search_or_create_node(const value_type &value)
	{
		return search_or_create(value, tree_root);
	}

	//----------------------------------------------------------
	inline AvlNode *search_node(const value_type &value) const
	{
		return search(value, (AvlNode*&)tree_root);
	}

	//----------------------------------------------------------
	void delete_node(const value_type &value)
	{
		delete_routine(value, tree_root);
	}

	//----------------------------------------------------------
	// (������ � ���������� ������)
	//----------------------------------------------------------
	void print_tree()
	{
#if defined _CONSOLE
		print_subtree(tree_root, 0);
#endif
	}

	//----------------------------------------------------------
	// print_subtree (������ � ���������� ������)
	//----------------------------------------------------------
	static void print_subtree(AvlNode *this_node, size_type node_depth);

	//----------------------------------------------------------
	// (��� ������������)
	//----------------------------------------------------------
	static bool check_balance_routine(
		AvlNode* node_to_check,
		size_type depth,
		size_type *depth_cells,
		size_type len
	);

	//----------------------------------------------------------
	// (��� ������������)
	//----------------------------------------------------------
	bool check_balance();

	//----------------------------------------------------------
	// ����� ����� � ������
	//----------------------------------------------------------
	size_type size() const
	{
		return node_counter;
	}

protected:
	virtual AvlNode* create_node(const value_type &value, AvlNode *&parent) = 0;

private:
	//----------------------------------------------------------
	// ������� ������
	//----------------------------------------------------------
	inline void destroy_tree()
	{
		destroy_subtree(tree_root);
	}

	//---------------------------------------------------
	void destroy_subtree(AvlNode *&subtree_root);

	//---------------------------------------------------
	// ��������������� �������, ������� ������ ���� ����,
	// �� ������ ������ �� ������
	//---------------------------------------------------
	static AvlNode **search_routine(
		register value_type value_to_search,
		AvlNode *&root,
		AvlNode *&out_parent_node
	);

	//---------------------------------------------------
	// �������� �������� ����
	//---------------------------------------------------
	void delete_routine(
		const value_type &value_to_delete,
		AvlNode *&root
	);

	//---------------------------------------------------
	// ������� ���� ���� � ������� ��� � ������,
	// ���� ���� � �������� ������ ���
	//---------------------------------------------------
	AvlNode * __fastcall search_or_create(
		const value_type &value_to_search,
		AvlNode *&root
	);

	//---------------------------------------------------
	// ������� ���� ����
	//---------------------------------------------------
	static AvlNode * __fastcall search(
		const value_type &value_to_search,
		AvlNode *&root
	);

private:
	AvlNode *tree_root;
	size_type node_counter;
};


/**********************
 *** Implementation ***
 **********************/
//----------------------------------------------------------
// ������� ��������� ����� ����� ������������
// � �������� �����
//----------------------------------------------------------
template <typename T, typename S>
void AvlTree<T, S>::AvlNode::destroy_parent_to_child_link(AvlNode *child)
{
	if (child)
	{
		AvlNode *this_parent = child->links.parent_node;
		child->links.parent_node = 0;

		if (this_parent)
			if (this_parent->links.left_node == child)
				this_parent->links.left_node = 0;
			else
				if (this_parent->links.right_node == child)
					this_parent->links.right_node = 0;
	}
}

//----------------------------------------------------------
// ��������� ����� ����� ����
//----------------------------------------------------------
template <typename T, typename S>
void AvlTree<T, S>::AvlNode::change_left(AvlNode *child, AvlNode *&out_child_old_parent, AvlNode *&out_parent_left_node)
{
	// ������ ������ �����
	out_child_old_parent = 0;
	out_parent_left_node = 0;

	if (links.left_node)
	{
		out_parent_left_node = links.left_node;
		links.left_node->links.parent_node = 0;
	}

	if (child)
	{
		out_child_old_parent = child->links.parent_node;
		destroy_parent_to_child_link(child);
	}

	// ��������� ����� �����
	links.left_node = child;
	if (child)
		child->links.parent_node = this;
}

//----------------------------------------------------------
// ��������� ������ ����� ����
//----------------------------------------------------------
template <typename T, typename S>
void AvlTree<T, S>::AvlNode::change_right(AvlNode *child, AvlNode *&out_child_old_parent, AvlNode *&out_parent_right_node)
{
	// ������ ������ �����
	out_child_old_parent = 0;
	out_parent_right_node = 0;

	if (links.right_node)
	{
		out_parent_right_node = links.right_node;
		links.right_node->links.parent_node = 0;
	}

	if (child)
	{
		out_child_old_parent = child->links.parent_node;
		destroy_parent_to_child_link(child);
	}

	// ��������� ����� �����
	links.right_node = child;
	if( child )
		child->links.parent_node = this;
}

//----------------------------------------------------------
// � ���������� ������ ������� new_node
// ������ ������� node_to_replace � �������� ������.
//----------------------------------------------------------
template <typename T, typename S>
void AvlTree<T, S>::AvlNode::replace_node(AvlNode *node_to_replace, AvlNode *new_node, AvlNode *&root, Links &new_node_old_links)
{
	// �������� ������ �����
	AvlNode *old_node_to_replace_parent = node_to_replace->links.parent_node;
	AvlNode *old_node_to_replace_parent_left = 0;
	AvlNode *old_node_to_replace_parent_right = 0;

	if (old_node_to_replace_parent)
	{
		old_node_to_replace_parent_left = old_node_to_replace_parent->links.left_node;
		old_node_to_replace_parent_right = old_node_to_replace_parent->links.right_node;
	}

	AvlNode *node_to_replace_tmp = 0; // node_to_replace

	// �������� ������ ����� new_node
	if (new_node)
	{
		// �������� �����
		new_node->change_left(node_to_replace->links.left_node, node_to_replace_tmp, new_node_old_links.left_node);
		new_node->change_right(node_to_replace->links.right_node, node_to_replace_tmp, new_node_old_links.right_node);
	}
	else
	{
		new_node_old_links.left_node = 0;
		new_node_old_links.right_node = 0;
		new_node_old_links.parent_node = 0;
	}

	if (old_node_to_replace_parent)
		if (node_to_replace == old_node_to_replace_parent_left)
			old_node_to_replace_parent->change_left(new_node, new_node_old_links.parent_node, old_node_to_replace_parent_left);
		else // if( node_to_replace == old_node_to_replace_parent_right )
			old_node_to_replace_parent->change_right(new_node, new_node_old_links.parent_node, old_node_to_replace_parent_right);
	else
	{// ���� ��� ������������� ����, ������ �� ����� �� �����
		root = new_node;

		if (new_node)
			destroy_parent_to_child_link(new_node);
	}
}

//----------------------------------------------------------
template <typename T, typename S>
void AvlTree<T, S>::AvlNode::rebalance_left_grew(AvlNode *node_to_balance, AvlNode *&root)
{
	rebalance_grew_routine(LEFT_IS_HEAVY, node_to_balance, root);
}

//----------------------------------------------------------
template <typename T, typename S>
void AvlTree<T, S>::AvlNode::rebalance_right_grew(AvlNode *node_to_balance, AvlNode *&root)
{
	rebalance_grew_routine(RIGHT_IS_HEAVY, node_to_balance, root);
}

//----------------------------------------------------------
template <typename T, typename S>
void AvlTree<T, S>::AvlNode::rebalance_grew_routine(typename AvlNode::Balanced grew, AvlNode *node_to_balance, AvlNode *&root)
{
	AvlNode *garbage = 0;
	AvlNode *child = 0;

	if (grew == LEFT_IS_HEAVY)
		child = node_to_balance->links.left_node;
	else
		if (grew == RIGHT_IS_HEAVY)
			child = node_to_balance->links.right_node;
		else
			return;

	if (child)
		if (child->balance == grew)
		{
			Links old_node_to_balance_links = node_to_balance->links;

			// ����, ������� �������� �� node_to_balance
			// ������ ������ ��������� �� child
			AvlNode::Links old_child_links;
			AvlNode::replace_node(node_to_balance, child, root, old_child_links);

			// �������� ������
			if (grew == LEFT_IS_HEAVY)
			{
				node_to_balance->change_left(old_child_links.right_node, garbage, garbage);
				node_to_balance->change_right(old_node_to_balance_links.right_node, garbage, garbage);

				child->change_right(node_to_balance, garbage, garbage);
				child->change_left(old_child_links.left_node, garbage, garbage);
			}
			else
			{
				node_to_balance->change_right(old_child_links.left_node, garbage, garbage);
				node_to_balance->change_left(old_node_to_balance_links.left_node, garbage, garbage);

				child->change_left(node_to_balance, garbage, garbage);
				child->change_right(old_child_links.right_node, garbage, garbage);
			}

			// ������ ������ ��������������
			node_to_balance->balance = BALANCED;
			node_to_balance = child;
		}
		else //if( child->balance == LEFT_IS_HEAVY )
		{
			// �������� ������-�����
			AvlTree::AvlNode *grandchild = 0;

			if (grew == LEFT_IS_HEAVY)
				grandchild = child->links.right_node;
			else
				grandchild = child->links.left_node;

			if (grandchild)
			{
				Links old_node_to_balance_links = node_to_balance->links;

				// ����, ������� �������� �� node_to_balance
				// ������ ������ ��������� �� grandchild
				AvlNode::Links old_grandchild_links;
				AvlNode::replace_node(node_to_balance, grandchild, root, old_grandchild_links);

				if (grew == LEFT_IS_HEAVY)
				{
					// �������� �����-������
					child->change_right(old_grandchild_links.left_node, garbage, garbage);

					node_to_balance->change_left(old_grandchild_links.right_node, garbage, garbage);
					node_to_balance->change_right(old_node_to_balance_links.right_node, garbage, garbage);

					grandchild->change_right(node_to_balance, garbage, garbage);
				}
				else
				{
					// �������� ������-�����
					child->change_left(old_grandchild_links.right_node, garbage, garbage);

					node_to_balance->change_right(old_grandchild_links.left_node, garbage, garbage);
					node_to_balance->change_left(old_node_to_balance_links.left_node, garbage, garbage);

					grandchild->change_left(node_to_balance, garbage, garbage);
				}

				if (grandchild->balance == grew)
					node_to_balance->balance = (Balanced)((int)grew * (-1));
				else
					node_to_balance->balance = BALANCED;

				if (grandchild->balance == (Balanced)((int)grew * (-1)))
					child->balance = grew;
				else
					child->balance = BALANCED;

				node_to_balance = grandchild;
			}
		}

	node_to_balance -> balance = AvlNode::BALANCED;
}

//----------------------------------------------------------
// ������� ���������� ������, ���� ���� �� ������
// ������������ �� �����
//----------------------------------------------------------
template <typename T, typename S>
bool AvlTree<T, S>::AvlNode::rebalance_left_shrunk(AvlNode *&node_to_balance, AvlNode *&root)
{
	return rebalance_shrunk_routine(LEFT_IS_HEAVY, node_to_balance, root);
}

//----------------------------------------------------------
// ������� ���������� ������, ���� ���� �� ������
// ������������ �� �����
//----------------------------------------------------------
template <typename T, typename S>
bool AvlTree<T, S>::AvlNode::rebalance_right_shrunk(AvlNode *&node_to_balance, AvlNode *&root)
{
	return rebalance_shrunk_routine(RIGHT_IS_HEAVY, node_to_balance, root);
}

//----------------------------------------------------------
// ������� ���������� ������, ���� ���� �� ������
// ������������ �� �����
//----------------------------------------------------------
template <typename T, typename S>
bool AvlTree<T, S>::AvlNode::rebalance_shrunk_routine(typename AvlNode::Balanced shrunk, AvlNode *&node_to_balance, AvlNode *&root)
{
	bool need_to_stop_balance = false;
	AvlNode *garbage = 0;

	// ����� ������ ���� �������
	if (node_to_balance->balance == shrunk)
		node_to_balance->balance = BALANCED; // �� ��������� ������� ����, ������ ��� ��������������
	else
		if (node_to_balance->balance == BALANCED)
		{
			// �� ������ ��������� ������� ����, ������ ������ �������
			node_to_balance->balance = (Balanced)((int)shrunk*(-1));

			// ������ � �������, ��� �� ���������� ������
			need_to_stop_balance = true;
		}
		else
		{// ����� ��������� ���� �������, ������ ���������
			AvlTree::AvlNode *child = 0;

			if (shrunk == LEFT_IS_HEAVY)
				child = node_to_balance->links.right_node;
			else
				child = node_to_balance->links.left_node;

			Balanced child_balance = child->balance;

			if (child->balance != shrunk)
			{
				Links old_node_to_balance_links = node_to_balance->links;

				// ����, ������� �������� �� node_to_balance
				// ������ ������ ��������� �� child
				AvlNode::Links old_child_links;
				AvlNode::replace_node(node_to_balance, child, root, old_child_links);

				if (shrunk == LEFT_IS_HEAVY)
				{
					// �������� ������
					node_to_balance->change_right(old_child_links.left_node, garbage, garbage);
					node_to_balance->change_left(old_node_to_balance_links.left_node, garbage, garbage);

					child->change_left(node_to_balance, garbage, garbage);
					child->change_right(old_child_links.right_node, garbage, garbage);
				}
				else
				{
					// �������� �����
					node_to_balance->change_left(old_child_links.right_node, garbage, garbage);
					node_to_balance->change_right(old_node_to_balance_links.right_node, garbage, garbage);

					child->change_right(node_to_balance, garbage, garbage);
					child->change_left(old_child_links.left_node, garbage, garbage);
				}

				if (child_balance == BALANCED)
				{
					node_to_balance->balance = (Balanced)((int)shrunk*(-1));
					child->balance = shrunk;

					// ���������� ������ �� ������
					need_to_stop_balance = true;
				}
				else
				{
					node_to_balance->balance = BALANCED;
					child->balance = BALANCED;
				}

				// ���������� ���� ����, ����� ��� ���� �� ������������� child
				node_to_balance = child;
			}
			else // ( child_balance == LEFT_IS_HEAVY ) || ( child_balance == BALANCED )
			{
				AvlTree::AvlNode *grandchild = 0;

				if (shrunk == LEFT_IS_HEAVY)
					grandchild = child->links.left_node;
				else
					grandchild = child->links.right_node;

				Balanced grandchild_balance = grandchild->balance;
				Links old_node_to_balance_links = node_to_balance->links;

				// ����, ������� �������� �� node_to_balance
				// ������ ������ ��������� �� grandchild
				AvlNode::Links old_grandchild_links;
				AvlNode::replace_node(node_to_balance, grandchild, root, old_grandchild_links);

				if (shrunk == LEFT_IS_HEAVY)
				{
					// �������� ������-�����
					child->change_left(old_grandchild_links.right_node, garbage, garbage);

					node_to_balance->change_right(old_grandchild_links.left_node, garbage, garbage);
					node_to_balance->change_left(old_node_to_balance_links.left_node, garbage, garbage);

					grandchild->change_left(node_to_balance, garbage, garbage);
				}
				else
				{
					// �������� �����-������
					child->change_right(old_grandchild_links.left_node, garbage, garbage);

					node_to_balance->change_left(old_grandchild_links.right_node, garbage, garbage);
					node_to_balance->change_right(old_node_to_balance_links.right_node, garbage, garbage);

					grandchild->change_right(node_to_balance, garbage, garbage);
				}

				if (grandchild_balance == (Balanced)((int)shrunk*(-1)))
					node_to_balance->balance = shrunk;
				else
					node_to_balance->balance = BALANCED;

				if (grandchild_balance == shrunk)
					child->balance = (Balanced)((int)shrunk*(-1));
				else
					child->balance = BALANCED;

				grandchild->balance = BALANCED;

				// ���������� ���� ����, ����� ��� ���� �� ������������� grandchild
				node_to_balance = grandchild;
			}
		}

	// ������ ��� �� ����������, ����� ������������ ����� �� ������
	return need_to_stop_balance;
}

//----------------------------------------------------------
// ������� ������������ ������, ������� �����������
// ��� ���������� ����.
//----------------------------------------------------------
template <typename T, typename S>
void AvlTree<T, S>::AvlNode::rebalance_grew(AvlNode *this_node, AvlNode *&root)
{
	// ����� ����������, ��������������� �� ������,
	// �� ������ �����, ����� ������ ���������
	// ����� �������

	// ���� ������ ���� �������� ������, ��, �� �����������,
	// �������������, ��� ��� ������ ���������� ���
	AvlNode *previous_node = this_node;
	this_node = this_node->links.parent_node;

	// ���� �� �������� �����
	while (this_node != 0)
	{
		// �� ��������� �� ������ ���������
		if (this_node->links.left_node == previous_node)
			if (this_node->balance == RIGHT_IS_HEAVY)
			{
				// ������ ������ ���� �������.
				// ������� �������� � ����� ���������,
				// ������� ������ ������ ��������������.
				this_node->balance = BALANCED;
				return;
			}
			else
				if (this_node->balance == BALANCED) // ������� ��������� �������� ����������������.
					this_node -> balance = LEFT_IS_HEAVY;
				else
				{// ������ ���������������� �����
					rebalance_left_grew(this_node, root);
					return;
				}
		else // �� ��������� �� ������� ���������
			if (this_node->links.right_node == previous_node)
				if (this_node -> balance == LEFT_IS_HEAVY)
				{
					// ����� ������ ���� �������.
					// ������� �������� � ������ ���������,
					// ������� ������ ������ ��������������.
					this_node->balance = BALANCED;
					return;
				}
				else
					if (this_node->balance == BALANCED) // ������� ��������� �������� ����������������.
						this_node->balance = RIGHT_IS_HEAVY;
					else
					{
						// ������ ���������������� ������
						rebalance_right_grew(this_node, root);
						return;
					}

		// ��������� � ������� ����� ������
		previous_node = this_node;
		this_node = this_node -> links.parent_node;
	}
}

//----------------------------------------------------------
// ������� ������������ ������, ������� �����������
// ��� �������� ����.
//----------------------------------------------------------
template <typename T, typename S>
void AvlTree<T, S>::AvlNode::rebalance_shrunk(AvlNode *this_node, value_type previous_node_value, AvlNode *&root)
{
	// ���� �� �������� �����
	while (this_node != 0)
	{
		// ����� ����������, ��������������� �� ������,
		// �� ������ �����, ����� ������ ���������
		// ����� ������

		// �� ��������� �� ������ ���������
		if (previous_node_value < this_node->value)
		{
			if (rebalance_left_shrunk(this_node, root))
				return;
		}
		else // �� ��������� �� ������� ���������
			if (this_node->value < previous_node_value)
				if (rebalance_right_shrunk(this_node, root))
					return;

		// ��������� � ������� ����� ������
		previous_node_value = this_node->value;
		this_node = this_node->links.parent_node;
	}
}

//----------------------------------------------------------
// print_subtree
// (�������� ������ � ���������� ������)
//----------------------------------------------------------
template <typename T, typename S>
void AvlTree<T, S>::print_subtree(AvlNode *this_node, size_type node_depth)
{
#if defined CONSOLE_AVLTREE
	if (this_node)
	{
		std::cout << "value = " << (long)this_node->value << ", balanced = " << this_node->balance<< std::endl;

		// ����� ���������
		// ������ ���������� ������� ������
		for (size_type i = 0; i < node_depth; ++i)
			std::cout << " ";

		std::cout << "left_node, ";
		print_subtree(this_node->links.left_node, node_depth + 1);

		// ������ ���������
		// ������ ���������� ������� ������
		for (size_type i = 0; i < node_depth; i++ )
			std::cout << " ";

		std::cout << "right_node, ";
		print_subtree(this_node->links.right_node, node_depth + 1);
	}
	else
		std::cout << std::endl;

#endif
}

//----------------------------------------------------------
// check_balance_routine
// (��� ������������)
//----------------------------------------------------------
template <typename T, typename S>
bool AvlTree<T, S>::check_balance_routine(AvlNode *node_to_check, size_type depth, size_type *depth_cells, size_type len)
{
#if defined(DEBUG_AVLTREE)
	// ��� ������� ���������
	if (node_to_check == 0)
	{
		for (size_type i = 0; i < len; ++i)
		{
			if (depth_cells[i] > 0)
			{
				if (depth == depth_cells[i])
					return true;
			}
			else
			{
				depth_cells[i] = depth;
				return true;
			}
		}

		return false;
	}
	else
	{
//		std::cout<< "node_to_check == " << (int) node_to_check->value << std::endl;
		return
		check_balance_routine(node_to_check->links.left_node, depth + 1, depth_cells, len) &&
		check_balance_routine(node_to_check->links.right_node, depth + 1, depth_cells, len);
	}
#else
	(void)node_to_check;
	(void)depth;
	(void)depth_cells;
	(void)len;

	return true;
#endif
}

//----------------------------------------------------------
// check_balance
// (��� ������������)
//----------------------------------------------------------
template <typename T, typename S>
bool AvlTree<T, S>::check_balance()
{
#if defined DEBUG_AVLTREE
	const size_type depth_cells_len = 8;
	size_type depth_cells[depth_cells_len] = { 0 };
	size_type depth = 0;

	bool is_balanced = check_balance_routine(tree_root, depth, depth_cells, depth_cells_len);

	for (size_type i = 0; i < depth_cells_len; ++i)
		std::cout << "depth_cells[" << i << "] == " << depth_cells[i] << std::endl;

	return is_balanced;

#else
	return true;
#endif
}

//---------------------------------------------------
template <typename T, typename S>
void AvlTree<T, S>::destroy_subtree(AvlNode *&subtree_root)
{
	if (!subtree_root)
		return;

	AvlNode *this_node = subtree_root;
	while (true)
		if (this_node->links.right_node)
			this_node = this_node->links.right_node;
		else
			if (this_node->links.left_node)
				this_node = this_node->links.left_node;
			else
			{
				AvlNode *node_to_delete = this_node;

				// ��������� �� ������� ����
				this_node = this_node->links.parent_node;

				// ������� ����
				delete node_to_delete;
				--node_counter;

				// ��� ��� ��������� ����, �������
				if (node_to_delete == subtree_root || this_node == 0)
					break;
				else
					// �� ������ ������
					if (node_to_delete == this_node->links.right_node)
						this_node->links.right_node = 0;
					else // ( node_to_delete == this_node->links.left_node )
						this_node->links.left_node = 0;
			}

	subtree_root = 0;
}

//---------------------------------------------------
// ��������������� �������, ������� ������ ���� ����,
// �� ������ ������ �� ������
//---------------------------------------------------
template <typename T, typename S>
typename AvlTree<T, S>::AvlNode** AvlTree<T, S>::search_routine(value_type value_to_search, AvlNode *&root, AvlNode *&out_parent_node)
{
	out_parent_node = 0;
	register AvlNode **this_node_pointer = &root;
	register AvlNode *this_parent_node = 0;

// ������������ �������������� �����������
// �� ����������, ���� � ������� 64-������ ��������, ��� ��� �������� �����
// ������ ��������� AVL_BINARY_NODE ����� �������.
#if (!defined AVL_SEARCH_TREE_DONT_OPTIMIZE) && (!defined __64BIT)

	_asm
	{
		// parent_node
		XOR ESI, ESI;

		MOV EDX, this_node_pointer;
		MOV EBX, value_to_search;

		// ���� �����
		_avl_search_routine_loop_begin:
		{
			MOV ECX, DWORD PTR[EDX]; // this_node, ��������� �� ������� ����

			// ���� �� ����� �� ����� ����� (������� ���������� �����)
			{
				TEST ECX, ECX;
				JE SHORT _avl_search_routine_loop_end;
			}

			MOV EAX, DWORD PTR [ECX+12]; // this_node->value, 12 - �������� value

			// ��� ���� �� ����� ���� (������� ���������� �����)
			{
				CMP EAX, EBX;
				JE SHORT _avl_search_routine_loop_end;
			}

			// parent_node
			MOV ESI, ECX;

			CMP EAX, EBX;

			// ���� ���������� ������� �������� ��������� - CMOV,
			// ����� ���������� �� �������� ���������

			// ��������� ������ �� ������
			LEA EAX, DWORD PTR [ECX+4]; // 4 - ��� �������� right_node � AVL_BINARY_NODE
			CMOVL EDX, EAX;

			// ��������� ����� �� ������
			LEA EAX, DWORD PTR [ECX+0]; // 0 - ��� �������� left_node � AVL_BINARY_NODE
			CMOVG EDX, EAX;
		}

		JMP SHORT _avl_search_routine_loop_begin;

		_avl_search_routine_loop_end:

		MOV this_parent_node, ESI;
		MOV this_node_pointer, EDX;
	}

	out_parent_node = this_parent_node;

#else // �� ������������ �������������� �����������

	// ���� �� ����� �� ����� �����
	while (*this_node_pointer && (*this_node_pointer)->value != value_to_search)
	{
		this_parent_node = *this_node_pointer;

		// ��������� ����� �� ������
		if (value_to_search < (*this_node_pointer)->value)
			this_node_pointer = &((*this_node_pointer)->links.left_node);
		else // if ( (*this_node_pointer)-> value < value_to_search )
			this_node_pointer = &((*this_node_pointer)->links.right_node);
	}

	out_parent_node = this_parent_node;

#endif

	// �� �����, ���������� �����
	// ���������, ����� ���������� ������� ����� ������� ����,
	// ���� ��� �����������
	return this_node_pointer;
}

//---------------------------------------------------
// �������� �������� ����
//---------------------------------------------------
template <typename T, typename S>
void AvlTree<T, S>::delete_routine(const value_type &value_to_delete, AvlNode *&root)
{
	AvlNode *garbage = 0;
	AvlNode *parent_node = 0;
	AvlNode *this_node = *search_routine(value_to_delete, root, parent_node);

	// ���� ������
	if (this_node)
	{
		typename AvlNode::Links old_this_node_links = this_node->links;
		AvlNode *node_to_balance = 0;
		value_type previous_node_value = 0;

		// ���� ����� ������ ������ �������
		// ������� ������ ������ �������
		if (this_node->links.right_node && this_node->links.left_node == 0)
		{
			typename AvlNode::Links old_links;
			AvlNode::replace_node(this_node, this_node->links.right_node, root, old_links);

			// ��������������� ��������� �����
			old_this_node_links.right_node->change_right(old_links.right_node, garbage, garbage);
			old_this_node_links.right_node->change_left(old_links.left_node, garbage, garbage);

			node_to_balance = old_this_node_links.right_node;
			previous_node_value = old_this_node_links.right_node->value;
		}
		else
			if (this_node->links.left_node && this_node->links.right_node == 0)
			{
				// ������� ������ ����� �������
				typename AvlNode::Links old_links;
				AvlNode::replace_node(this_node, this_node -> links.left_node, root, old_links);

				// ��������������� ��������� �����
				old_this_node_links.left_node->change_right(old_links.right_node, garbage, garbage);
				old_this_node_links.left_node->change_left(old_links.left_node, garbage, garbage);

				node_to_balance = old_this_node_links.left_node;
				previous_node_value = old_this_node_links.left_node->value;
			}
			else
				if (this_node->links.left_node && this_node->links.right_node)
				{
					// ����, ������� ������� �������
					AvlNode *this_replace_node = this_node->links.left_node;

					// ����� �������� ���� �� ����� ������� �������
					if (this_node->links.left_node->links.right_node == 0)
					{
						// ������ ����� �������� � ����� ���������
						node_to_balance = this_replace_node;
						previous_node_value = (value_type)(((long)this_replace_node->value) - 1);
					}
					else
						node_to_balance = 0;

					// ������ �� ������ �������� ������� ������� ������ ����
					while (this_replace_node->links.right_node != 0)
						this_replace_node = this_replace_node->links.right_node;

					this_replace_node->balance = this_node->balance;
					AvlNode *deep_right_parent = this_replace_node->links.parent_node;

					typename AvlNode::Links old_replace_node_links;
					AvlNode::replace_node(this_node, this_replace_node, root, old_replace_node_links);

					// � ������ ���� ��� ������ �������. ���� ��, �� ������� ����� ������� ������ ���� ��� ����� ��������
					if (node_to_balance == 0)
						deep_right_parent->change_right(old_replace_node_links.left_node, garbage, garbage);
					else
						this_replace_node->change_left(old_replace_node_links.left_node, garbage, garbage);

					// � ������ ��������� ���� ���� ������ �������
					if (node_to_balance == 0)
					{
						// ������ ����� �������� � �������� ������� ������� ������ ����
						node_to_balance = deep_right_parent;
						previous_node_value = (value_type)(((long)deep_right_parent->value) + 1);
					}
				}
				else
				{// ���� �� ����� ��������
					typename AvlNode::Links old_replace_node_links;
					AvlNode::replace_node(this_node, 0, root, old_replace_node_links);

					node_to_balance = old_this_node_links.parent_node;
					previous_node_value = this_node->value;
				}

		if (node_to_balance)
		{
			AvlNode::rebalance_shrunk(node_to_balance, previous_node_value, root);
			destroy_node(this_node);
			--node_counter;
		}
	}
}

//---------------------------------------------------
// ������� ���� ���� � ������� ��� � ������,
// ���� ���� � �������� ������ ���
//---------------------------------------------------
template <typename T, typename S>
typename AvlTree<T, S>::AvlNode* AvlTree<T, S>::search_or_create(const value_type &value_to_search, AvlNode *&root)
{
	AvlNode *parent_node = 0;
	AvlNode **this_node = search_routine(value_to_search, root, parent_node);
	AvlNode *node_to_return = *this_node;

	// �����
	if (*this_node)
		return node_to_return;
	else
	{
		if ((node_to_return = *this_node = create_node(value_to_search, parent_node)))
		{
			++node_counter;
			// ��������� �� �������� ����� ����, �������� � ������
			// ��������� ���������, ���������� ����������������
			AvlNode::rebalance_grew(*this_node, root);
		}

		return node_to_return;
	}
}

//---------------------------------------------------
// ������� ���� ����
//---------------------------------------------------
template <typename T, typename S>
typename AvlTree<T, S>::AvlNode *AvlTree<T, S>::search(const value_type &value_to_search, AvlNode *&root)
{
	AvlNode *parent_node = 0;
	return *search_routine(value_to_search, root, parent_node);
}

#endif /* AVLTREE_H_ */
