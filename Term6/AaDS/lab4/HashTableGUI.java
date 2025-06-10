import javax.swing.*;
import java.awt.*;

public class HashTableGUI extends JFrame {
    private final HashTable<String> table = new HashTable<>(17);
    private final JTextField keyField = new JTextField(10);
    private final JTextField valueField = new JTextField(10);
    private final JTextArea outputArea = new JTextArea(10, 40);
    private final JTable tableView = new JTable();

    private final JComboBox<String> operationBox = new JComboBox<>(new String[]{
            "Вставить", "Удалить", "Поиск", "Очистить",
            "Опрос размера", "Опрос количества элементов", "Проверка на пустоту",
            "Структура таблицы", "Оценка хеширования (χ²)"
    });

    public HashTableGUI() {
        setTitle("Хеш-таблица с открытой адресацией");
        setSize(900, 600);
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setLayout(new BorderLayout());

        JPanel controlPanel = new JPanel();
        controlPanel.add(new JLabel("Операция:"));
        controlPanel.add(operationBox);
        controlPanel.add(new JLabel("Ключ:"));
        controlPanel.add(keyField);
        controlPanel.add(new JLabel("Значение:"));
        controlPanel.add(valueField);
        JButton executeBtn = new JButton("Выполнить");
        controlPanel.add(executeBtn);

        add(controlPanel, BorderLayout.NORTH);

        JTabbedPane tabs = new JTabbedPane();
        JScrollPane outputScroll = new JScrollPane(outputArea);
        outputArea.setEditable(false);
        tabs.addTab("Результаты", outputScroll);

        JScrollPane tableScroll = new JScrollPane(tableView);
        tabs.addTab("Хеш-таблица", tableScroll);

        add(tabs, BorderLayout.CENTER);

        operationBox.addActionListener(e -> updateInputVisibility());

        executeBtn.addActionListener(e -> performOperation());

        updateInputVisibility();
	pack();
    }

    private void updateInputVisibility() {
        String op = (String) operationBox.getSelectedItem();
        keyField.setText("");
        valueField.setText("");

        boolean keyVisible = switch (op) {
            case "Вставить", "Удалить", "Поиск" -> true;
            default -> false;
        };
        boolean valueVisible = op.equals("Вставить");

        keyField.setVisible(keyVisible);
        valueField.setVisible(valueVisible);
    }

    private void performOperation() {
        String op = (String) operationBox.getSelectedItem();
        String key = keyField.getText().trim();
        String value = valueField.getText().trim();

        try {
            switch (op) {
                case "Вставить" -> {
                    boolean ok = table.insert(key, value);
                    show("Результат вставки: " + (ok ? "успешно" : "ошибка (возможно дубликат)"));
                }
                case "Удалить" -> {
                    boolean ok = table.remove(key);
                    show("Результат удаления: " + (ok ? "успешно" : "ключ не найден"));
                }
                case "Поиск" -> {
                    String found = table.find(key);
                    show(found != null ? "Найдено значение: " + found : "Ключ не найден");
                }
                case "Очистить" -> {
                    table.clear();
                    show("Таблица очищена");
                }
                case "Опрос размера" -> show("Размер таблицы: " + table.capacity());
                case "Опрос количества элементов" -> show("Элементов в таблице: " + table.size());
                case "Проверка на пустоту" -> show(table.isEmpty() ? "Таблица пуста" : "В таблице есть элементы");
                case "Структура таблицы" -> show(table.structureString());
                case "Оценка хеширования (χ²)" -> {
                    double chi2 = HashStats.chiSquaredEstimate(table.capacity(), 1000, 20);
                    show("χ²-оценка равномерности хеширования: " + String.format("%.2f", chi2));
                }
            }
        } catch (Exception ex) {
            show("Ошибка: " + ex.getMessage());
        }
    }

    private void show(String msg) {
        outputArea.setText(msg);
    }
}
