#ifndef AVLTREE_H_
#define AVLTREE_H_


/*****************
 *** Interface ***
 *****************/
template <typename Key, typename SizeType = unsigned int>
class AvlTree
{
public:
	typedef Key      key_type;
	typedef SizeType size_type;

	//----------------------------------------------------------
	// AvlNode
	//----------------------------------------------------------
	struct AvlNode
	{
		struct Links {AvlNode *left; AvlNode *right; AvlNode *parent;} links;
		enum Balanced {BALANCED = 0, LEFT_IS_HEAVY = -1, RIGHT_IS_HEAVY = 1} balance;
		key_type key;

		//----------------------------------------------------------
		AvlNode(const key_type &value, AvlNode *parent_node) :
			key(value),
			balance(BALANCED)
		{
			links.left = 0;
			links.right = 0;
			links.parent = parent_node;
		}
		virtual ~AvlNode() {}

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
	};

	//----------------------------------------------------------
	AvlTree() :
		m_treeRoot(0),
		m_nodeCounter(0)
	{}

	//----------------------------------------------------------
	virtual ~AvlTree()
	{
		destroy_subtree(m_treeRoot);
	}

	//----------------------------------------------------------
	inline AvlNode *search_or_create_node(const key_type &value)
	{
		return search_or_create(value, m_treeRoot);
	}

	//----------------------------------------------------------
	inline AvlNode *search_node(const key_type &value) const
	{
		return search(value, (AvlNode*&)m_treeRoot);
	}

	//----------------------------------------------------------
	void delete_node(const key_type &value)
	{
		delete_routine(value, m_treeRoot);
	}

	//----------------------------------------------------------
	// ����� ����� � ������
	//----------------------------------------------------------
	size_type size() const
	{
		return m_nodeCounter;
	}

protected:
	virtual AvlNode* create_node(const key_type &value, AvlNode *&parent) = 0;

private:
	//----------------------------------------------------------
	void destroy_parent_to_child_link(AvlNode *child);

	//----------------------------------------------------------
	// � ���������� ������ ������� new_node
	// ������ ������� node_to_replace � �������� ������.
	//----------------------------------------------------------
	void replace_node(
		AvlNode *node_to_replace,
		AvlNode *new_node,
		AvlNode *&root,
		// �������� ��������
		typename AvlNode::Links &new_node_old_links
	);

	//----------------------------------------------------------
	void rebalance_left_grew(
		AvlNode *node_to_balance,
		AvlNode *&root
	);

	//----------------------------------------------------------
	void rebalance_right_grew(
		AvlNode *node_to_balance,
		AvlNode *&root
	);

	//----------------------------------------------------------
	void rebalance_grew_routine(
		typename AvlNode::Balanced grew,
		AvlNode *node_to_balance,
		AvlNode *&root
	);

	//----------------------------------------------------------
	bool rebalance_left_shrunk(
		AvlNode *&node_to_balance,
		AvlNode *&root
	);

	//----------------------------------------------------------
	bool rebalance_right_shrunk(
		AvlNode *&node_to_balance,
		AvlNode *&root
	);

	//----------------------------------------------------------
	bool rebalance_shrunk_routine(
		typename AvlNode::Balanced shrunk,
		AvlNode *&node_to_balance,
		AvlNode *&root
	);

	//----------------------------------------------------------
	// ���������������� ��� ���������� ����
	//----------------------------------------------------------
	void rebalance_grew(
		AvlNode *this_node,
		AvlNode *&root );

	//----------------------------------------------------------
	// ���������������� ��� �������� ����
	//----------------------------------------------------------
	void rebalance_shrunk(
		AvlNode *this_node,
		key_type previous_node_value,
		AvlNode *&root
	);


	//----------------------------------------------------------
	// ������� ������
	//----------------------------------------------------------
	inline void destroy_tree()
	{
		destroy_subtree(m_treeRoot);
	}

	//---------------------------------------------------
	void destroy_subtree(AvlNode *&subtree_root);

	//---------------------------------------------------
	// ��������������� �������, ������� ������ ���� ����,
	// �� ������ ������ �� ������
	//---------------------------------------------------
	AvlNode *&search_routine(
		const key_type &key,
		AvlNode *&root,
		AvlNode *&out_parent_node
	);

	//---------------------------------------------------
	// �������� �������� ����
	//---------------------------------------------------
	void delete_routine(
		const key_type &key,
		AvlNode *&root
	);

	//---------------------------------------------------
	// ������� ���� ���� � ������� ��� � ������,
	// ���� ���� � �������� ������ ���
	//---------------------------------------------------
	AvlNode * __fastcall search_or_create(
		const key_type &key,
		AvlNode *&root
	);

	//---------------------------------------------------
	// ������� ���� ����
	//---------------------------------------------------
	AvlNode * __fastcall search(
		const key_type &key,
		AvlNode *&root
	);

private:
	AvlNode *m_treeRoot;
	size_type m_nodeCounter;
};


/**********************
 *** Implementation ***
 **********************/
//----------------------------------------------------------
// ������� ��������� ����� ����� ������������
// � �������� �����
//----------------------------------------------------------
template <typename T, typename S>
void AvlTree<T, S>::destroy_parent_to_child_link(AvlNode *child)
{
	if (child)
	{
		AvlNode *this_parent = child->links.parent;
		child->links.parent = 0;

		if (this_parent)
			if (this_parent->links.left == child)
				this_parent->links.left = 0;
			else
				if (this_parent->links.right == child)
					this_parent->links.right = 0;
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

	if (links.left)
	{
		out_parent_left_node = links.left;
		links.left->links.parent = 0;
	}

	if (child)
	{
		out_child_old_parent = child->links.parent;
		destroy_parent_to_child_link(child);
	}

	// ��������� ����� �����
	links.left = child;
	if (child)
		child->links.parent = this;
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

	if (links.right)
	{
		out_parent_right_node = links.right;
		links.right->links.parent = 0;
	}

	if (child)
	{
		out_child_old_parent = child->links.parent;
		destroy_parent_to_child_link(child);
	}

	// ��������� ����� �����
	links.right = child;
	if( child )
		child->links.parent = this;
}

//----------------------------------------------------------
// � ���������� ������ ������� new_node
// ������ ������� node_to_replace � �������� ������.
//----------------------------------------------------------
template <typename T, typename S>
void AvlTree<T, S>::replace_node(AvlNode *node_to_replace, AvlNode *new_node, AvlNode *&root, typename AvlNode::Links &new_node_old_links)
{
	// �������� ������ �����
	AvlNode *old_node_to_replace_parent = node_to_replace->links.parent;
	AvlNode *old_node_to_replace_parent_left = 0;
	AvlNode *old_node_to_replace_parent_right = 0;

	if (old_node_to_replace_parent)
	{
		old_node_to_replace_parent_left = old_node_to_replace_parent->links.left;
		old_node_to_replace_parent_right = old_node_to_replace_parent->links.right;
	}

	AvlNode *node_to_replace_tmp = 0; // node_to_replace

	// �������� ������ ����� new_node
	if (new_node)
	{
		// �������� �����
		new_node->change_left(node_to_replace->links.left, node_to_replace_tmp, new_node_old_links.left);
		new_node->change_right(node_to_replace->links.right, node_to_replace_tmp, new_node_old_links.right);
	}
	else
	{
		new_node_old_links.left = 0;
		new_node_old_links.right = 0;
		new_node_old_links.parent = 0;
	}

	if (old_node_to_replace_parent)
		if (node_to_replace == old_node_to_replace_parent_left)
			old_node_to_replace_parent->change_left(new_node, new_node_old_links.parent, old_node_to_replace_parent_left);
		else
			old_node_to_replace_parent->change_right(new_node, new_node_old_links.parent, old_node_to_replace_parent_right);
	else
	{
		// ���� ��� ������������� ����, ������ �� ����� �� �����
		root = new_node;

		if (new_node)
			destroy_parent_to_child_link(new_node);
	}
}

//----------------------------------------------------------
template <typename T, typename S>
void AvlTree<T, S>::rebalance_left_grew(AvlNode *node_to_balance, AvlNode *&root)
{
	rebalance_grew_routine(AvlNode::LEFT_IS_HEAVY, node_to_balance, root);
}

//----------------------------------------------------------
template <typename T, typename S>
void AvlTree<T, S>::rebalance_right_grew(AvlNode *node_to_balance, AvlNode *&root)
{
	rebalance_grew_routine(AvlNode::RIGHT_IS_HEAVY, node_to_balance, root);
}

//----------------------------------------------------------
template <typename T, typename S>
void AvlTree<T, S>::rebalance_grew_routine(typename AvlNode::Balanced grew, AvlNode *node_to_balance, AvlNode *&root)
{
	AvlNode *garbage = 0;
	AvlNode *child = 0;

	if (grew == AvlNode::LEFT_IS_HEAVY)
		child = node_to_balance->links.left;
	else
		if (grew == AvlNode::RIGHT_IS_HEAVY)
			child = node_to_balance->links.right;
		else
			return;

	if (child)
		if (child->balance == grew)
		{
			typename AvlNode::Links old_node_to_balance_links = node_to_balance->links;

			// ����, ������� �������� �� node_to_balance ������ ������ ��������� �� child
			typename AvlNode::Links old_child_links;
			replace_node(node_to_balance, child, root, old_child_links);

			// �������� ������
			if (grew == AvlNode::LEFT_IS_HEAVY)
			{
				node_to_balance->change_left(old_child_links.right, garbage, garbage);
				node_to_balance->change_right(old_node_to_balance_links.right, garbage, garbage);

				child->change_right(node_to_balance, garbage, garbage);
				child->change_left(old_child_links.left, garbage, garbage);
			}
			else
			{
				node_to_balance->change_right(old_child_links.left, garbage, garbage);
				node_to_balance->change_left(old_node_to_balance_links.left, garbage, garbage);

				child->change_left(node_to_balance, garbage, garbage);
				child->change_right(old_child_links.right, garbage, garbage);
			}

			// ������ ������ ��������������
			node_to_balance->balance = AvlNode::BALANCED;
			node_to_balance = child;
		}
		else //if( child->balance == LEFT_IS_HEAVY )
		{
			// �������� ������-�����
			AvlTree::AvlNode *grandchild = 0;

			if (grew == AvlNode::LEFT_IS_HEAVY)
				grandchild = child->links.right;
			else
				grandchild = child->links.left;

			if (grandchild)
			{
				typename AvlNode::Links old_node_to_balance_links = node_to_balance->links;

				// ����, ������� �������� �� node_to_balance
				// ������ ������ ��������� �� grandchild
				typename AvlNode::Links old_grandchild_links;
				replace_node(node_to_balance, grandchild, root, old_grandchild_links);

				if (grew == AvlNode::LEFT_IS_HEAVY)
				{
					// �������� �����-������
					child->change_right(old_grandchild_links.left, garbage, garbage);

					node_to_balance->change_left(old_grandchild_links.right, garbage, garbage);
					node_to_balance->change_right(old_node_to_balance_links.right, garbage, garbage);

					grandchild->change_right(node_to_balance, garbage, garbage);
				}
				else
				{
					// �������� ������-�����
					child->change_left(old_grandchild_links.right, garbage, garbage);

					node_to_balance->change_right(old_grandchild_links.left, garbage, garbage);
					node_to_balance->change_left(old_node_to_balance_links.left, garbage, garbage);

					grandchild->change_left(node_to_balance, garbage, garbage);
				}

				if (grandchild->balance == grew)
					node_to_balance->balance = (AvlNode::Balanced)((int)grew * (-1));
				else
					node_to_balance->balance = AvlNode::BALANCED;

				if (grandchild->balance == (AvlNode::Balanced)((int)grew * (-1)))
					child->balance = grew;
				else
					child->balance = AvlNode::BALANCED;

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
bool AvlTree<T, S>::rebalance_left_shrunk(AvlNode *&node_to_balance, AvlNode *&root)
{
	return rebalance_shrunk_routine(AvlNode::LEFT_IS_HEAVY, node_to_balance, root);
}

//----------------------------------------------------------
// ������� ���������� ������, ���� ���� �� ������
// ������������ �� �����
//----------------------------------------------------------
template <typename T, typename S>
bool AvlTree<T, S>::rebalance_right_shrunk(AvlNode *&node_to_balance, AvlNode *&root)
{
	return rebalance_shrunk_routine(AvlNode::RIGHT_IS_HEAVY, node_to_balance, root);
}

//----------------------------------------------------------
// ������� ���������� ������, ���� ���� �� ������
// ������������ �� �����
//----------------------------------------------------------
template <typename T, typename S>
bool AvlTree<T, S>::rebalance_shrunk_routine(typename AvlNode::Balanced shrunk, AvlNode *&node_to_balance, AvlNode *&root)
{
	bool need_to_stop_balance = false;
	AvlNode *garbage = 0;

	// ����� ������ ���� �������
	if (node_to_balance->balance == shrunk)
		node_to_balance->balance = AvlNode::BALANCED; // �� ��������� ������� ����, ������ ��� ��������������
	else
		if (node_to_balance->balance == AvlNode::BALANCED)
		{
			// �� ������ ��������� ������� ����, ������ ������ �������
			node_to_balance->balance = (AvlNode::Balanced)((int)shrunk*(-1));

			// ������ � �������, ��� �� ���������� ������
			need_to_stop_balance = true;
		}
		else
		{
			// ����� ��������� ���� �������, ������ ���������
			AvlTree::AvlNode *child = 0;

			if (shrunk == AvlNode::LEFT_IS_HEAVY)
				child = node_to_balance->links.right;
			else
				child = node_to_balance->links.left;

			typename AvlNode::Balanced child_balance = child->balance;

			if (child->balance != shrunk)
			{
				typename AvlNode::Links old_node_to_balance_links = node_to_balance->links;

				// ����, ������� �������� �� node_to_balance
				// ������ ������ ��������� �� child
				typename AvlNode::Links old_child_links;
				replace_node(node_to_balance, child, root, old_child_links);

				if (shrunk == AvlNode::LEFT_IS_HEAVY)
				{
					// �������� ������
					node_to_balance->change_right(old_child_links.left, garbage, garbage);
					node_to_balance->change_left(old_node_to_balance_links.left, garbage, garbage);

					child->change_left(node_to_balance, garbage, garbage);
					child->change_right(old_child_links.right, garbage, garbage);
				}
				else
				{
					// �������� �����
					node_to_balance->change_left(old_child_links.right, garbage, garbage);
					node_to_balance->change_right(old_node_to_balance_links.right, garbage, garbage);

					child->change_right(node_to_balance, garbage, garbage);
					child->change_left(old_child_links.left, garbage, garbage);
				}

				if (child_balance == AvlNode::BALANCED)
				{
					node_to_balance->balance = (AvlNode::Balanced)((int)shrunk*(-1));
					child->balance = shrunk;

					// ���������� ������ �� ������
					need_to_stop_balance = true;
				}
				else
				{
					node_to_balance->balance = AvlNode::BALANCED;
					child->balance = AvlNode::BALANCED;
				}

				// ���������� ���� ����, ����� ��� ���� �� ������������� child
				node_to_balance = child;
			}
			else
			{
				AvlTree::AvlNode *grandchild = 0;

				if (shrunk == AvlNode::LEFT_IS_HEAVY)
					grandchild = child->links.left;
				else
					grandchild = child->links.right;

				typename AvlNode::Balanced grandchild_balance = grandchild->balance;
				typename AvlNode::Links old_node_to_balance_links = node_to_balance->links;

				// ����, ������� �������� �� node_to_balance
				// ������ ������ ��������� �� grandchild
				typename AvlNode::Links old_grandchild_links;
				replace_node(node_to_balance, grandchild, root, old_grandchild_links);

				if (shrunk == AvlNode::LEFT_IS_HEAVY)
				{
					// �������� ������-�����
					child->change_left(old_grandchild_links.right, garbage, garbage);

					node_to_balance->change_right(old_grandchild_links.left, garbage, garbage);
					node_to_balance->change_left(old_node_to_balance_links.left, garbage, garbage);

					grandchild->change_left(node_to_balance, garbage, garbage);
				}
				else
				{
					// �������� �����-������
					child->change_right(old_grandchild_links.left, garbage, garbage);

					node_to_balance->change_left(old_grandchild_links.right, garbage, garbage);
					node_to_balance->change_right(old_node_to_balance_links.right, garbage, garbage);

					grandchild->change_right(node_to_balance, garbage, garbage);
				}

				if (grandchild_balance == (AvlNode::Balanced)((int)shrunk*(-1)))
					node_to_balance->balance = shrunk;
				else
					node_to_balance->balance = AvlNode::BALANCED;

				if (grandchild_balance == shrunk)
					child->balance = (AvlNode::Balanced)((int)shrunk*(-1));
				else
					child->balance = AvlNode::BALANCED;

				grandchild->balance = AvlNode::BALANCED;

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
void AvlTree<T, S>::rebalance_grew(AvlNode *this_node, AvlNode *&root)
{
	// ����� ����������, ��������������� �� ������,
	// �� ������ �����, ����� ������ ���������
	// ����� �������

	// ���� ������ ���� �������� ������, ��, �� �����������,
	// �������������, ��� ��� ������ ���������� ���
	AvlNode *previous_node = this_node;
	this_node = this_node->links.parent;

	// ���� �� �������� �����
	while (this_node != 0)
	{
		// �� ��������� �� ������ ���������
		if (this_node->links.left == previous_node)
			if (this_node->balance == AvlNode::RIGHT_IS_HEAVY)
			{
				// ������ ������ ���� �������.
				// ������� �������� � ����� ���������,
				// ������� ������ ������ ��������������.
				this_node->balance = AvlNode::BALANCED;
				return;
			}
			else
				if (this_node->balance == AvlNode::BALANCED) // ������� ��������� �������� ����������������.
					this_node->balance = AvlNode::LEFT_IS_HEAVY;
				else
				{
					// ������ ���������������� �����
					rebalance_left_grew(this_node, root);
					return;
				}
		else // �� ��������� �� ������� ���������
			if (this_node->links.right == previous_node)
				if (this_node->balance == AvlNode::LEFT_IS_HEAVY)
				{
					// ����� ������ ���� �������.
					// ������� �������� � ������ ���������,
					// ������� ������ ������ ��������������.
					this_node->balance = AvlNode::BALANCED;
					return;
				}
				else
					if (this_node->balance == AvlNode::BALANCED) // ������� ��������� �������� ����������������.
						this_node->balance = AvlNode::RIGHT_IS_HEAVY;
					else
					{
						// ������ ���������������� ������
						rebalance_right_grew(this_node, root);
						return;
					}

		// ��������� � ������� ����� ������
		previous_node = this_node;
		this_node = this_node->links.parent;
	}
}

//----------------------------------------------------------
// ������� ������������ ������, ������� �����������
// ��� �������� ����.
//----------------------------------------------------------
template <typename T, typename S>
void AvlTree<T, S>::rebalance_shrunk(AvlNode *this_node, key_type previous_node_value, AvlNode *&root)
{
	// ���� �� �������� �����
	while (this_node != 0)
	{
		// ����� ����������, ��������������� �� ������,
		// �� ������ �����, ����� ������ ���������
		// ����� ������

		// �� ��������� �� ������ ���������
		if (previous_node_value < this_node->key)
		{
			if (rebalance_left_shrunk(this_node, root))
				return;
		}
		else // �� ��������� �� ������� ���������
			if (this_node->key < previous_node_value)
				if (rebalance_right_shrunk(this_node, root))
					return;

		// ��������� � ������� ����� ������
		previous_node_value = this_node->key;
		this_node = this_node->links.parent;
	}
}

//---------------------------------------------------
template <typename T, typename S>
void AvlTree<T, S>::destroy_subtree(AvlNode *&subtree_root)
{
	if (!subtree_root)
		return;

	AvlNode *this_node = subtree_root;
	while (true)
		if (this_node->links.right)
			this_node = this_node->links.right;
		else
			if (this_node->links.left)
				this_node = this_node->links.left;
			else
			{
				AvlNode *node_to_delete = this_node;

				// ��������� �� ������� ����
				this_node = this_node->links.parent;

				// ������� ����
				delete node_to_delete;
				--m_nodeCounter;

				// ��� ��� ��������� ����, �������
				if (node_to_delete == subtree_root || this_node == 0)
					break;
				else
					// �� ������ ������
					if (node_to_delete == this_node->links.right)
						this_node->links.right = 0;
					else
						this_node->links.left = 0;
			}

	subtree_root = 0;
}

//---------------------------------------------------
// ��������������� �������, ������� ������ ���� ����,
// �� ������ ������ �� ������
//---------------------------------------------------
template <typename T, typename S>
typename AvlTree<T, S>::AvlNode *&AvlTree<T, S>::search_routine(const key_type &key, AvlNode *&root, AvlNode *&out_parent_node)
{
	AvlNode *&this_node_pointer = root;
	AvlNode *this_parent_node = 0;

	// ���� �� ����� �� ����� �����
	while (this_node_pointer && this_node_pointer->key != key)
	{
		this_parent_node = this_node_pointer;

		if (key < this_node_pointer->key)
			this_node_pointer = this_node_pointer->links.left;
		else
			this_node_pointer = this_node_pointer->links.right;
	}

	out_parent_node = this_parent_node;

	// �� �����, ���������� �����
	// ���������, ����� ���������� ������� ����� ������� ����,
	// ���� ��� �����������
	return this_node_pointer;
}

//---------------------------------------------------
// �������� �������� ����
//---------------------------------------------------
template <typename T, typename S>
void AvlTree<T, S>::delete_routine(const key_type &key, AvlNode *&root)
{
	AvlNode *garbage = 0;
	AvlNode *parent_node = 0;
	AvlNode *this_node = *search_routine(key, root, parent_node);

	// ���� ������
	if (this_node)
	{
		typename AvlNode::Links old_this_node_links = this_node->links;
		AvlNode *node_to_balance = 0;
		key_type previous_node_value = 0;

		// ���� ����� ������ ������ �������
		// ������� ������ ������ �������
		if (this_node->links.right && this_node->links.left == 0)
		{
			typename AvlNode::Links old_links;
			AvlNode::replace_node(this_node, this_node->links.right, root, old_links);

			// ��������������� ��������� �����
			old_this_node_links.right->change_right(old_links.right, garbage, garbage);
			old_this_node_links.right->change_left(old_links.left, garbage, garbage);

			node_to_balance = old_this_node_links.right;
			previous_node_value = old_this_node_links.right->key;
		}
		else
			if (this_node->links.left && this_node->links.right == 0)
			{
				// ������� ������ ����� �������
				typename AvlNode::Links old_links;
				AvlNode::replace_node(this_node, this_node -> links.left, root, old_links);

				// ��������������� ��������� �����
				old_this_node_links.left->change_right(old_links.right, garbage, garbage);
				old_this_node_links.left->change_left(old_links.left, garbage, garbage);

				node_to_balance = old_this_node_links.left;
				previous_node_value = old_this_node_links.left->key;
			}
			else
				if (this_node->links.left && this_node->links.right)
				{
					// ����, ������� ������� �������
					AvlNode *this_replace_node = this_node->links.left;

					// ����� �������� ���� �� ����� ������� �������
					if (this_node->links.left->links.right == 0)
					{
						// ������ ����� �������� � ����� ���������
						node_to_balance = this_replace_node;
						previous_node_value = (key_type)(((long)this_replace_node->key) - 1);
					}
					else
						node_to_balance = 0;

					// ������ �� ������ �������� ������� ������� ������ ����
					while (this_replace_node->links.right != 0)
						this_replace_node = this_replace_node->links.right;

					this_replace_node->balance = this_node->balance;
					AvlNode *deep_right_parent = this_replace_node->links.parent;

					typename AvlNode::Links old_replace_node_links;
					AvlNode::replace_node(this_node, this_replace_node, root, old_replace_node_links);

					// � ������ ���� ��� ������ �������. ���� ��, �� ������� ����� ������� ������ ���� ��� ����� ��������
					if (node_to_balance == 0)
						deep_right_parent->change_right(old_replace_node_links.left, garbage, garbage);
					else
						this_replace_node->change_left(old_replace_node_links.left, garbage, garbage);

					// � ������ ��������� ���� ���� ������ �������
					if (node_to_balance == 0)
					{
						// ������ ����� �������� � �������� ������� ������� ������ ����
						node_to_balance = deep_right_parent;
						previous_node_value = (key_type)(((long)deep_right_parent->key) + 1);
					}
				}
				else
				{// ���� �� ����� ��������
					typename AvlNode::Links old_replace_node_links;
					AvlNode::replace_node(this_node, 0, root, old_replace_node_links);

					node_to_balance = old_this_node_links.parent;
					previous_node_value = this_node->key;
				}

		if (node_to_balance)
		{
			AvlNode::rebalance_shrunk(node_to_balance, previous_node_value, root);
			destroy_node(this_node);
			--m_nodeCounter;
		}
	}
}

//---------------------------------------------------
// ������� ���� ���� � ������� ��� � ������,
// ���� ���� � �������� ������ ���
//---------------------------------------------------
template <typename T, typename S>
typename AvlTree<T, S>::AvlNode* AvlTree<T, S>::search_or_create(const key_type &key, AvlNode *&root)
{
	AvlNode *parent_node = 0;
	AvlNode *&this_node = search_routine(key, root, parent_node);

	// �����
	if (this_node)
		return this_node;
	else
	{
		if ((this_node = create_node(key, parent_node)))
		{
			++m_nodeCounter;
			// ��������� �� �������� ����� ����, �������� � ������
			// ��������� ���������, ���������� ����������������
			AvlNode::rebalance_grew(this_node, root);
		}

		return this_node;
	}
}

//---------------------------------------------------
// ������� ���� ����
//---------------------------------------------------
template <typename T, typename S>
typename AvlTree<T, S>::AvlNode *AvlTree<T, S>::search(const key_type &key, AvlNode *&root)
{
	AvlNode *parent_node = 0;
	return *search_routine(key, root, parent_node);
}

#endif /* AVLTREE_H_ */
