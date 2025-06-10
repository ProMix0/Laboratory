import javax.swing.*;
import java.awt.*;

public class TreePanel<K extends Comparable<K>, V> extends JPanel {
    private final TwoThreeTree<K, V> tree;

    public TreePanel(TwoThreeTree<K, V> tree) {
        this.tree = tree;
        setPreferredSize(new Dimension(800, 400));
        setBackground(Color.WHITE);
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        drawTree(g, tree.getRoot(), getWidth() / 2, 40, getWidth() / 4);
    }

    private void drawTree(Graphics g, TwoThreeTree<K, V>.Node node, int x, int y, int xOffset) {
        if (node == null) return;

        String label = node.isTwoNode
                ? "[" + node.key1 + "]"
                : "[" + node.key1 + ", " + node.key2 + "]";

        int width = node.isTwoNode ? 40 : 70;
        g.drawRect(x - width / 2, y - 15, width, 30);
        g.drawString(label, x - width / 2 + 5, y + 5);

        if (node.left != null) {
            int childX = x - xOffset;
            int childY = y + 60;
            g.drawLine(x, y + 15, childX, childY - 15);
            drawTree(g, node.left, childX, childY, xOffset / 2);
        }

        if (node.middle != null) {
            int childX = x;
            int childY = y + 60;
            g.drawLine(x, y + 15, childX, childY - 15);
            drawTree(g, node.middle, childX, childY, xOffset / 2);
        }

        if (!node.isTwoNode && node.right != null) {
            int childX = x + xOffset;
            int childY = y + 60;
            g.drawLine(x, y + 15, childX, childY - 15);
            drawTree(g, node.right, childX, childY, xOffset / 2);
        }
    }
}
