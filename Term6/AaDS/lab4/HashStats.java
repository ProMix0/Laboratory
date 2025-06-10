import java.util.Random;

public class HashStats {
    public static double chiSquaredEstimate(int tableSize, int insertions, int experiments) {
        double sum = 0.0;
        for (int exp = 0; exp < experiments; exp++) {
            int[] bins = new int[tableSize];
            Random rand = new Random();

            for (int i = 0; i < insertions; i++) {
                String key = randomKey(rand);
                long keyBits = transformKey(key);
                int index = multiplicativeHash(keyBits, tableSize);
                bins[index]++;
            }

            double expected = (double) insertions / tableSize;
            double chi2 = 0.0;
            for (int count : bins) {
                double diff = count - expected;
                chi2 += (diff * diff) / expected;
            }
            sum += chi2;
        }
        return sum / experiments;
    }

    private static String randomKey(Random rand) {
        int len = rand.nextInt(5) + 3;
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < len; i++)
            sb.append((char) ('A' + rand.nextInt(26)));
        return sb.toString();
    }

    private static long transformKey(String key) {
        long bits = 0;
        for (char c : key.toCharArray()) {
            bits = (bits << 5) | (c - 'A');
        }
	if (bits < 0) bits = -bits;
        return bits;
    }

    private static int multiplicativeHash(long keyBits, int capacity) {
        double A = 0.6180339887;
        double frac = (keyBits * A) % 1;
        return (int) (capacity * frac);
    }
}
