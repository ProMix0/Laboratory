import javax.swing.*;
import java.awt.*;
import java.util.Map;

public class BSTreeFullGUI extends JFrame {
    private final BSTree<Integer, String> tree = new BSTree<>();
    private final BSTreePanel<Integer, String> treePanel = new BSTreePanel<>(tree);
    private final JTextArea outputArea = new JTextArea(10, 70);

    private final JTextField keyField = new JTextField(10);
    private final JTextField valueField = new JTextField(10);

    private final JComboBox<String> operationBox = new JComboBox<>(new String[]{
            "Вставить", "Удалить", "Поиск", "Изменить значение",
            "Очистить", "Проверка на пустоту", "Опрос размера",
            "Обход (Lt-t-Rt)"
    });

    public BSTreeFullGUI() {
        setTitle("BST-Дерево (полный интерфейс)");
        setSize(900, 600);
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setLayout(new BorderLayout());

        // Верхняя панель управления
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

        // Центр: вкладки с деревом и результатами
        JTabbedPane tabs = new JTabbedPane();
        tabs.addTab("Визуализация дерева", new JScrollPane(treePanel));
        tabs.addTab("Результаты", new JScrollPane(outputArea));
        outputArea.setEditable(false);
        add(tabs, BorderLayout.CENTER);

        // Видимость полей по выбранной операции
        operationBox.addActionListener(e -> updateFieldVisibility());

        // Выполнение операций
        executeBtn.addActionListener(e -> performOperation());

        updateFieldVisibility(); // начальная инициализация
	pack();
    }

    private void updateFieldVisibility() {
        String op = (String) operationBox.getSelectedItem();
        keyField.setText("");
        valueField.setText("");

        boolean keyNeeded = switch (op) {
            case "Вставить", "Удалить", "Поиск", "Изменить значение" -> true;
            default -> false;
        };
        boolean valueNeeded = switch (op) {
            case "Вставить", "Изменить значение" -> true;
            default -> false;
        };

        keyField.setVisible(keyNeeded);
        valueField.setVisible(valueNeeded);
    }

    private void performOperation() {
        String op = (String) operationBox.getSelectedItem();
        try {
            int key = keyField.isVisible() ? Integer.parseInt(keyField.getText()) : 0;
            String val = valueField.isVisible() ? valueField.getText() : null;

            switch (op) {
                case "Вставить" -> {
                    tree.insert(key, val);
                    showMessage("Вставлен ключ " + key);
                }
                case "Удалить" -> {
                    tree.remove(key);
                    showMessage("Удалён ключ " + key);
                }
                case "Поиск" -> {
                    String found = tree.get(key);
                    showMessage(found != null ? "Найдено: " + found : "Ключ не найден");
                }
                case "Изменить значение" -> {
                    tree.set(key, val);
                    showMessage("Значение по ключу " + key + " обновлено");
                }
                case "Очистить" -> {
                    tree.clear();
                    showMessage("Дерево очищено");
                }
                case "Проверка на пустоту" -> showMessage(tree.isEmpty() ? "Дерево пусто" : "Дерево не пусто");
                case "Опрос размера" -> showMessage("Размер дерева: " + tree.size());
                case "Обход (Lt-t-Rt)" -> {
                    var sb = new StringBuilder("Обход (Lt–t–Rt):\n");
                    for (Map.Entry<Integer, String> entry : tree.inOrder())
                        sb.append(entry.getKey()).append(" -> ").append(entry.getValue()).append("\n");
                    showMessage(sb.toString());
                }
            }

            treePanel.repaint();
        } catch (Exception ex) {
            showMessage("Ошибка: " + ex.getMessage());
        }
    }

    private void showMessage(String text) {
        outputArea.setText(text);
    }
}
