import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class ListGUI extends JFrame {
    private final CircularLinkedList<String> list = new CircularLinkedList<String>();
    private final JTextArea outputArea = new JTextArea(10, 70);

    private final JComboBox<String> operationBox = new JComboBox<>(new String[]{
            "Добавить", "Добавить в позицию", "Удалить по значению", "Удалить по индексу",
            "Чтение по индексу", "Найти индекс по значению", "Проверка наличия", "Очистить список"
    });

    private final JLabel valueLabel = new JLabel("Значение:");
    private final JTextField input1 = new JTextField(10);

    private final JLabel indexLabel = new JLabel("Позиция:");
    private final JTextField input2 = new JTextField(10);

    private final JButton executeBtn = new JButton("Выполнить");

    public ListGUI() {
        setTitle("Кольцевой список (ОДС)");
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setLayout(new BorderLayout());

        JPanel topPanel = new JPanel();
        topPanel.add(operationBox);
        topPanel.add(valueLabel);
        topPanel.add(input1);
        topPanel.add(indexLabel);
        topPanel.add(input2);
        topPanel.add(executeBtn);

        outputArea.setEditable(false);
        add(topPanel, BorderLayout.NORTH);
        add(new JScrollPane(outputArea), BorderLayout.CENTER);

        operationBox.addActionListener(e -> updateInputVisibility());

        executeBtn.addActionListener(e -> {
            String op = (String) operationBox.getSelectedItem();
            String val = input1.getText();
            String idxText = input2.getText();

            try {
                switch (op) {
                    case "Добавить" -> list.insert(val);
                    case "Добавить в позицию" -> list.insertAt(val, Integer.parseInt(idxText));
                    case "Удалить по значению" -> list.remove(val);
                    case "Удалить по индексу" -> list.removeAt(Integer.parseInt(idxText));
                    case "Чтение по индексу" -> show("Значение: " + list.get(Integer.parseInt(idxText)));
                    case "Найти индекс по значению" -> show("Индекс: " + list.indexOf(val));
                    case "Проверка наличия" -> show("Содержит: " + list.contains(val));
                    case "Очистить список" -> list.clear();
                }
                refreshList();
            } catch (Exception ex) {
                show("Ошибка: " + ex.toString());
            }
        });

        updateInputVisibility(); // начальное скрытие/показ
        refreshList();
        setVisible(true);
	pack();
    }

    private void updateInputVisibility() {
        String op = (String) operationBox.getSelectedItem();

        // сброс значений
        input1.setText("");
        input2.setText("");

        // Управление видимостью
        valueLabel.setVisible(false);
        input1.setVisible(false);
        indexLabel.setVisible(false);
        input2.setVisible(false);

        switch (op) {
            case "Добавить" -> {
                valueLabel.setVisible(true);
                input1.setVisible(true);
            }
            case "Добавить в позицию" -> {
                valueLabel.setVisible(true);
                input1.setVisible(true);
                indexLabel.setVisible(true);
                input2.setVisible(true);
            }
            case "Удалить по значению" -> {
                valueLabel.setVisible(true);
                input1.setVisible(true);
            }
            case "Удалить по индексу", "Чтение по индексу" -> {
                indexLabel.setVisible(true);
                input2.setVisible(true);
            }
            case "Найти индекс по значению", "Проверка наличия" -> {
                valueLabel.setVisible(true);
                input1.setVisible(true);
            }
            case "Очистить список" -> {
                // ничего не нужно
            }
        }

        revalidate();
        repaint();
    }

    private void refreshList() {
        StringBuilder sb = new StringBuilder("Текущий список: [ ");
        for (String item : list) sb.append("\"").append(item).append("\" ");
        sb.append("]");
        outputArea.setText(sb.toString());
    }

    private void show(String msg) {
        JOptionPane.showMessageDialog(this, msg);
    }
}
