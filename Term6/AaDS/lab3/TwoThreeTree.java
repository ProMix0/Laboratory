import java.util.*;

public class TwoThreeTree<K extends Comparable<K>, V> {
    public class Node {
        K key1, key2;
        V val1, val2;
        Node left, middle, right;
        boolean isTwoNode;

        Node(K key, V val) {
            key1 = key;
            val1 = val;
            isTwoNode = true;
        }

        boolean isLeaf() {
            return left == null && middle == null && right == null;
        }

        int keyCount() {
            return isTwoNode ? 1 : 2;
        }
    }

    private Node root;
    private int size = 0;

    public Node getRoot() {
        return root;
    }

    public TwoThreeTree() {}

    public void clear() {
        root = null;
        size = 0;
    }

    public boolean isEmpty() {
        return size == 0;
    }

    public int size() {
        return size;
    }

    public V get(K key) {
        Node current = root;
        while (current != null) {
            int cmp1 = key.compareTo(current.key1);
            if (cmp1 == 0) return current.val1;
            if (!current.isTwoNode) {
                int cmp2 = key.compareTo(current.key2);
                if (cmp2 == 0) return current.val2;
                if (cmp1 < 0) current = current.left;
                else if (cmp2 < 0) current = current.middle;
                else current = current.right;
            } else {
                if (cmp1 < 0) current = current.left;
                else current = current.middle;
            }
        }
        return null;
    }

    private class InsertResult {
        Node newNode;
        K upKey;
        V upVal;

        InsertResult(Node node, K key, V val) {
            newNode = node;
            upKey = key;
            upVal = val;
        }
    }

    public void insert(K key, V value) {
        InsertResult result = insert(root, key, value);
        if (result != null && result.upKey != null) {
            Node newRoot = new Node(result.upKey, result.upVal);
            newRoot.left = result.newNode;
            newRoot.middle = result.newNode.right;
            root = newRoot;
        } else if (result != null) {
            root = result.newNode;
        }
        size++;
    }

    private InsertResult insert(Node node, K key, V value) {
        if (node == null) return new InsertResult(new Node(key, value), null, null);
        if (node.isLeaf()) return mergeLeaf(node, key, value);
        if (node.isTwoNode) {
            int cmp = key.compareTo(node.key1);
            InsertResult res = cmp < 0 ? insert(node.left, key, value)
                    : insert(node.middle, key, value);
            if (res != null && res.upKey != null)
                return mergeInternalTwo(node, res, cmp < 0);
            return null;
        } else {
            int cmp1 = key.compareTo(node.key1);
            int cmp2 = key.compareTo(node.key2);
            InsertResult res;
            if (cmp1 < 0) {
                res = insert(node.left, key, value);
                if (res != null && res.upKey != null) return splitThree(node, res, 0);
            } else if (cmp2 < 0) {
                res = insert(node.middle, key, value);
                if (res != null && res.upKey != null) return splitThree(node, res, 1);
            } else {
                res = insert(node.right, key, value);
                if (res != null && res.upKey != null) return splitThree(node, res, 2);
            }
            return null;
        }
    }

    private InsertResult mergeLeaf(Node node, K key, V value) {
        int cmp = key.compareTo(node.key1);
        if (node.isTwoNode) {
            if (cmp == 0) node.val1 = value;
            else if (cmp < 0) {
                node.key2 = node.key1; node.val2 = node.val1;
                node.key1 = key; node.val1 = value;
                node.isTwoNode = false;
            } else {
                node.key2 = key; node.val2 = value;
                node.isTwoNode = false;
            }
            return null;
        } else {
            ArrayList<K> keys = new ArrayList<>(Arrays.asList(node.key1, node.key2, key));
            ArrayList<V> vals = new ArrayList<>(Arrays.asList(node.val1, node.val2, value));
            for (int i = 0; i < 2; i++) {
                if (keys.get(i).compareTo(keys.get(i + 1)) > 0) {
                    Collections.swap(keys, i, i + 1);
                    Collections.swap(vals, i, i + 1);
                }
            }
            Node left = new Node(keys.get(0), vals.get(0));
            Node right = new Node(keys.get(2), vals.get(2));
            return new InsertResult(left, keys.get(1), vals.get(1)) {{
                newNode.right = right;
            }};
        }
    }

    private InsertResult mergeInternalTwo(Node node, InsertResult child, boolean leftSide) {
        if (leftSide) {
            node.key2 = node.key1; node.val2 = node.val1;
            node.key1 = child.upKey; node.val1 = child.upVal;
            node.right = node.middle;
            node.left = child.newNode;
            node.middle = child.newNode.right;
        } else {
            node.key2 = child.upKey; node.val2 = child.upVal;
            node.right = child.newNode.right;
            node.middle = child.newNode;
        }
        node.isTwoNode = false;
        return null;
    }

    private InsertResult splitThree(Node node, InsertResult child, int pos) {
        ArrayList<K> keys = new ArrayList<>(List.of(node.key1, node.key2, child.upKey));
        ArrayList<V> vals = new ArrayList<>(List.of(node.val1, node.val2, child.upVal));
        ArrayList<Node> children = new ArrayList<>(List.of(node.left, node.middle, node.right));
        children.add(pos, child.newNode);
        children.set(pos + 1, child.newNode.right);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2 - i; j++) {
                if (keys.get(j).compareTo(keys.get(j + 1)) > 0) {
                    Collections.swap(keys, j, j + 1);
                    Collections.swap(vals, j, j + 1);
                    Collections.swap(children, j + 1, j + 2);
                }
            }
        }

        Node left = new Node(keys.get(0), vals.get(0));
        Node right = new Node(keys.get(2), vals.get(2));
        left.left = children.get(0); left.middle = children.get(1);
        right.left = children.get(2); right.middle = children.get(3);
        return new InsertResult(left, keys.get(1), vals.get(1)) {{
            newNode.right = right;
        }};
    }

    public void remove(K key) {
        root = remove(root, key);
        if (root != null && !root.isTwoNode && root.left == null && root.middle == null && root.right == null) {
            root.key1 = root.key2;
            root.val1 = root.val2;
            root.key2 = null;
            root.val2 = null;
            root.isTwoNode = true;
        }
        if (root != null && root.isLeaf() && root.key1 == null) {
            root = null;
        }
        size = Math.max(0, size - 1);
    }

    private Node remove(Node node, K key) {
        if (node == null || node.key1 == null) return null;

        if (node.isLeaf()) {
            if (node.isTwoNode) {
                if (key.equals(node.key1)) return null;
            } else {
                if (key.equals(node.key1)) {
                    node.key1 = node.key2;
                    node.val1 = node.val2;
                    node.key2 = null;
                    node.val2 = null;
                    node.isTwoNode = true;
                    return node;
                } else if (key.equals(node.key2)) {
                    node.key2 = null;
                    node.val2 = null;
                    node.isTwoNode = true;
                    return node;
                }
            }
            return node;
        }

        if (key.equals(node.key1) || (!node.isTwoNode && key.equals(node.key2))) {
            if (node.isTwoNode) {
                Node successor = getMin(node.middle);
                node.key1 = successor.key1;
                node.val1 = successor.val1;
                node.middle = remove(node.middle, successor.key1);
            } else {
                if (key.equals(node.key1)) {
                    Node pred = getMax(node.left);
                    node.key1 = pred.key2 != null ? pred.key2 : pred.key1;
                    node.val1 = pred.key2 != null ? pred.val2 : pred.val1;
                    node.left = remove(node.left, node.key1);
                } else {
                    Node succ = getMin(node.right);
                    node.key2 = succ.key1;
                    node.val2 = succ.val1;
                    node.right = remove(node.right, succ.key1);
                }
            }
        } else {
            int cmp1 = key.compareTo(node.key1);
            if (cmp1 < 0) {
                node.left = remove(node.left, key);
            } else if (node.isTwoNode || key.compareTo(node.key2) < 0) {
                node.middle = remove(node.middle, key);
            } else {
                node.right = remove(node.right, key);
            }
        }

        return node;
    }

    private Node getMin(Node node) {
        while (node != null && node.left != null) {
            node = node.left;
        }
        return node;
    }

    private Node getMax(Node node) {
        while (node != null && (node.right != null || node.middle != null)) {
            node = (node.right != null) ? node.right : node.middle;
        }
        return node;
    }
}
