import java.util.Iterator;
import java.util.NoSuchElementException;

public class CircularLinkedList<T> implements Iterable<T> {
    private class Node {
        T data;
        Node next;

        public Node(T data) {
            this.data = data;
            this.next = null;
        }
    }
    private Node head;
    private int size;

    public CircularLinkedList() {
        head = new Node(null);
	head.next = head;
        size = 0;
    }

    public void clear() {
        head.next = head;
        size = 0;
    }

    public boolean isEmpty() {
        return size == 0;
    }

    public int size() {
        return size;
    }

    public boolean contains(T value) {
        for (T item : this) {
            if (item.equals(value)) return true;
        }
        return false;
    }

    public T get(int index) {
        checkIndex(index);
        Node current = head.next;
        for (int i = 0; i < index; i++) current = current.next;
        return current.data;
    }

    public int indexOf(T value) {
        Node current = head.next;
        for (int i = 0; i < size; i++) {
            if (current.data.equals(value)) return i;
            current = current.next;
        }
        return -1;
    }

    public void insert(T value) {
        insertAt(value, size);
    }

    public void insertAt(T value, int index) {
        if (index < 0 || index > size) throw new IndexOutOfBoundsException(index);
        Node newNode = new Node(value);
	Node current = head;
	while (index-- > 0) current = current.next;

        newNode.next = current.next;
	current.next = newNode;

        size++;
    }

    public void remove(T value) {
        if (isEmpty()) return;
        Node current = head.next;
        Node prev = head;

        for (int i = 0; i < size; i++) {
            if (current.data.equals(value)) {
                prev.next = current.next;
                size--;
                return;
            }
            prev = current;
            current = current.next;
        }
    }

    public void removeAt(int index) {
        checkIndex(index);
        Node current = head.next;
        Node prev = head;
        for (int i = 0; i < index; i++) {
            prev = current;
            current = current.next;
        }

        prev.next = current.next;
        size--;
    }

    private void checkIndex(int index) {
        if (index < 0 || index >= size) throw new IndexOutOfBoundsException(index);
    }

    @Override
    public Iterator<T> iterator() {
        return new Iterator<>() {
            Node current = head.next;

            public boolean hasNext() {
                return current != head;
            }

            public T next() {
                if (!hasNext()) throw new NoSuchElementException();
                T data = current.data;
                current = current.next;
                return data;
            }
        };
    }
}
