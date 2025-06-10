import java.util.ArrayList;

public class HashTable<V> {
    private class Entry {
        String key;
        V value;
        boolean isDeleted;

        Entry(String key, V value) {
            this.key = key;
            this.value = value;
            this.isDeleted = false;
        }
    }

    private ArrayList<Entry> table;
    private int capacity;
    private int size;
    private final double A = 0.6180339887; // константа для мультипликативного хеширования

    public HashTable(int capacity) {
        this.capacity = capacity;
        this.size = 0;
        this.table = new ArrayList<Entry>(capacity);
	for (int i = 0; i < capacity; i++)
	    table.add(null);
    }

    public int size() {
        return size;
    }

    public int capacity() {
        return capacity;
    }

    public boolean isEmpty() {
        return size == 0;
    }

    public void clear() {
	for (int i = 0; i < capacity; i++)
	    table.set(i, null);
        size = 0;
    }

    private long transformKey(String key) {
        long bits = 0;
        for (char c : key.toCharArray()) {
            bits = (bits << 5) | (c - 'A'); // 5 битов на символ (0–25)
        }
	if (bits < 0) bits = -bits;
        return bits;
    }

    // Мультипликативное хеширование
    private int hash(long keyBits) {
        double fractional = (keyBits * A) % 1;
        return (int) (capacity * fractional);
    }

    // Вставка
    public boolean insert(String key, V value) {
        int index = hash(transformKey(key));

        for (int i = 0; i < capacity; i++) {
            int idx = probe(index, i);
            if (table.get(idx) == null || table.get(idx).isDeleted) {
                table.set(idx, new Entry(key, value));
                size++;
                return true;
            } else if (table.get(idx).key.compareTo(key) == 0) {
                return false; // дубликат
            }
        }
        return false; // переполнение
    }

    // Поиск
    public V find(String key) {
        int index = hash(transformKey(key));

        for (int i = 0; i < capacity; i++) {
            int idx = probe(index, i);
            Entry e = table.get(idx);
            if (e == null) return null;
            if (!e.isDeleted && e.key.compareTo(key) == 0) {
                return e.value;
            }
        }
        return null;
    }

    // Удаление
    public boolean remove(String key) {
        int index = hash(transformKey(key));

        for (int i = 0; i < capacity; i++) {
            int idx = probe(index, i);
            Entry e = table.get(idx);
            if (e == null) return false;
            if (!e.isDeleted && e.key.compareTo(key) == 0) {
                e.isDeleted = true;
                size--;
                return true;
            }
        }
        return false;
    }

    // Квадратичное зондирование
    private int probe(int hash, int i) {
        return (hash + i * i) % capacity;
    }

    // Вывод структуры
    public String structureString() {
        StringBuilder sb = new StringBuilder("Хеш-таблица:\n");
        for (int i = 0; i < capacity; i++) {
            Entry e = table.get(i);
            sb.append(i).append(": ");
            if (e == null) sb.append("[пусто]");
            else if (e.isDeleted) sb.append("[удалено]");
            else sb.append(e.key).append(" → ").append(e.value);
            sb.append("\n");
        }
        return sb.toString();
    }
}

