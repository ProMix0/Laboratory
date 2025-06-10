import java.util.*;

public class BSTree<K extends Comparable<K>, V> {
    static class Node<K, V> {
        K key;
        V value;
        Node<K, V> left, right, parent;

        Node(K key, V value, Node<K, V> parent) {
            this.key = key;
            this.value = value;
	    this.parent = parent;
        }
    }

    private Node<K, V> root;
    private int size;

    public BSTree() {
        root = null;
        size = 0;
    }
    public int size() {
        return size;
    }

    public boolean isEmpty() {
        return size == 0;
    }

    public void clear() {
        root = null;
        size = 0;
    }

    public V get(K key) {
        Node<K, V> node = findNode(key);
        return node != null ? node.value : null;
    }

    public void set(K key, V value) {
        Node<K, V> node = findNode(key);
        if (node != null) node.value = value;
    }

    public void insert(K key, V value) {
        if (root == null) {
            root = new Node<>(key, value, null);
            size++;
            return;
        }

        Node<K, V> current = root, parent = null;
        while (current != null) {
            parent = current;
            int cmp = key.compareTo(current.key);
            if (cmp < 0) current = current.left;
            else if (cmp > 0) current = current.right;
            else return; // ключ уже существует
        }

        Node<K, V> newNode = new Node<>(key, value, parent);
        if (key.compareTo(parent.key) < 0) parent.left = newNode;
        else parent.right = newNode;
        size++;
    }

    public void remove(K key) {
        Node<K, V> node = findNode(key);
        if (node == null) return;

        if (node.left != null && node.right != null) {
            Node<K, V> successor = findMin(node.right);
            node.key = successor.key;
            node.value = successor.value;
            node = successor;
        }

        Node<K, V> child = (node.left != null) ? node.left : node.right;
        if (child != null) child.parent = node.parent;

        if (node.parent == null) root = child;
        else if (node == node.parent.left) node.parent.left = child;
        else node.parent.right = child;

        size--;
    }

    private Node<K, V> findNode(K key) {
        Node<K, V> current = root;
        while (current != null) {
            int cmp = key.compareTo(current.key);
            if (cmp == 0) return current;
            current = cmp < 0 ? current.left : current.right;
        }
        return null;
    }

    private Node<K, V> findMin(Node<K, V> node) {
        while (node.left != null) {
            node = node.left;
        }
        return node;
    }

    private Node<K, V> findMax(Node<K, V> node) {
        while (node.right != null) {
            node = node.right;
        }
        return node;
    }

    public List<Map.Entry<K, V>> inOrder() {
        List<Map.Entry<K, V>> result = new ArrayList<>();
        Stack<Node<K, V>> stack = new Stack<>();
        Node<K, V> current = root;

        while (current != null || !stack.isEmpty()) {
            while (current != null) {
                stack.push(current);
                current = current.left;
            }
            current = stack.pop();
            result.add(Map.entry(current.key, current.value));
            current = current.right;
        }

        return result;
    }

    public Node<K, V> getRoot() {
        return root;
    }
}

