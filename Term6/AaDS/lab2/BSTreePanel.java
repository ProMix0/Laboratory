import javax.swing.*;
import java.awt.*;

public class BSTreePanel<K extends Comparable<K>, V> extends JPanel {
    private final BSTree<K, V> tree;

    public BSTreePanel(BSTree<K, V> tree) {
        this.tree = tree;
        setPreferredSize(new Dimension(800, 400));
        setBackground(Color.WHITE);
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        BSTree.Node<K, V> root = tree.getRoot();
        if (root != null) drawNode(g, root, getWidth() / 2, 30, getWidth() / 4);
    }

    private void drawNode(Graphics g, BSTree.Node<K, V> node, int x, int y, int offset) {
        g.setColor(Color.BLACK);
        g.drawOval(x - 20, y - 20, 40, 40);
        g.drawString(String.valueOf(node.key), x - 10, y + 5);

        if (node.left != null) {
            g.drawLine(x, y, x - offset, y + 60);
            drawNode(g, node.left, x - offset, y + 60, offset / 2);
        }

        if (node.right != null) {
            g.drawLine(x, y, x + offset, y + 60);
            drawNode(g, node.right, x + offset, y + 60, offset / 2);
        }
    }
}