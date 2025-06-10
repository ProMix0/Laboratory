import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class TwoThreeTreeGUI extends JFrame {
    private final TwoThreeTree<String, String> tree = new TwoThreeTree<>();
    private final JTextArea output = new JTextArea();
    private final JComboBox<String> operationBox = new JComboBox<>(new String[]{
            "Добавить", "Найти", "Удалить", "Очистить", "Размер", "Проверка на пустоту"
    });

    private final JLabel keyLabel = new JLabel("Ключ:");
    private final JTextField keyField = new JTextField(10);

    private final JLabel valueLabel = new JLabel("Значение:");
    private final JTextField valueField = new JTextField(10);

    private final JButton executeButton = new JButton("Выполнить");
    private final TreePanel<String, String> treePanel = new TreePanel<>(tree);

    public TwoThreeTreeGUI() {
        super("2-3 Дерево (сбалансированное)");

        // Верхняя панель управления
        JPanel controlPanel = new JPanel();
        controlPanel.setLayout(new FlowLayout(FlowLayout.LEFT));

        controlPanel.add(new JLabel("Операция:"));
        controlPanel.add(operationBox);
        controlPanel.add(keyLabel);
        controlPanel.add(keyField);
        controlPanel.add(valueLabel);
        controlPanel.add(valueField);
        controlPanel.add(executeButton);

        // Панель вывода
        output.setEditable(false);
        output.setFont(new Font("Monospaced", Font.PLAIN, 12));
        JScrollPane scrollPane = new JScrollPane(output);
        scrollPane.setPreferredSize(new Dimension(800, 120));

        // Расположение в окне
        setLayout(new BorderLayout());
        add(controlPanel, BorderLayout.NORTH);
        add(treePanel, BorderLayout.CENTER);
        add(scrollPane, BorderLayout.SOUTH);

        // Слушатели
        operationBox.addActionListener(e -> updateFieldVisibility());
        executeButton.addActionListener(e -> performOperation());

        updateFieldVisibility();

        setSize(800, 600);
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setVisible(true);
	pack();
    }

    private void updateFieldVisibility() {
        String op = (String) operationBox.getSelectedItem();

        keyLabel.setVisible(false);
        keyField.setVisible(false);
        valueLabel.setVisible(false);
        valueField.setVisible(false);

        switch (op) {
            case "Добавить":
                keyLabel.setVisible(true);
                keyField.setVisible(true);
                valueLabel.setVisible(true);
                valueField.setVisible(true);
                break;
            case "Найти":
            case "Удалить":
                keyLabel.setVisible(true);
                keyField.setVisible(true);
                break;
        }

        revalidate();
        repaint();
    }

    private void performOperation() {
        String op = (String) operationBox.getSelectedItem();
        String key = keyField.getText().trim();
        String value = valueField.getText().trim();

        switch (op) {
            case "Добавить":
                if (!key.isEmpty()) {
                    tree.insert(key, value);
                    output.append("Добавлено: " + key + " → " + value + "\n");
                } else {
                    output.append("Введите ключ\n");
                }
                break;
            case "Найти":
                if (!key.isEmpty()) {
                    String result = tree.get(key);
                    output.append("Найдено: " + key + " → " + (result != null ? result : "не найдено") + "\n");
                } else {
                    output.append("Введите ключ для поиска.\n");
                }
                break;
            case "Удалить":
                if (!key.isEmpty()) {
                    tree.remove(key);
                    output.append("Удалено: " + key + "\n");
                } else {
                    output.append("Введите ключ для удаления.\n");
                }
                break;
            case "Очистить":
                tree.clear();
                output.append("Дерево очищено\n");
                break;
            case "Размер":
                output.append("Размер дерева: " + tree.size() + "\n");
                break;
            case "Проверка на пустоту":
                output.append("Дерево пусто? " + (tree.isEmpty() ? "Да" : "Нет") + "\n");
                break;
        }

        keyField.setText("");
        valueField.setText("");
        treePanel.repaint();
    }
}
