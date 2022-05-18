package analog.com.IoTNode;

import android.support.p000v4.view.ViewCompat;
import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.GridLabelRenderer;
import com.jjoe64.graphview.LegendRenderer;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.LineGraphSeries;
import java.util.ArrayList;

public class Utilities {
    public static int[] signedToUnsigned(byte[] bytes) {
        int[] new_byte = new int[bytes.length];
        for (int i = 0; i < bytes.length; i++) {
            new_byte[i] = bytes[i] & 255;
        }
        return new_byte;
    }

    public static void setGraphVisuals(GraphView graph) {
        graph.getGridLabelRenderer().setGridColor(ViewCompat.MEASURED_STATE_MASK);
        graph.getGridLabelRenderer().setHorizontalLabelsColor(ViewCompat.MEASURED_STATE_MASK);
        graph.getGridLabelRenderer().setVerticalLabelsColor(ViewCompat.MEASURED_STATE_MASK);
        graph.getGridLabelRenderer().setHorizontalAxisTitleColor(ViewCompat.MEASURED_STATE_MASK);
        graph.getGridLabelRenderer().setVerticalAxisTitleColor(ViewCompat.MEASURED_STATE_MASK);
        graph.getGridLabelRenderer().setVerticalLabelsSecondScaleColor(ViewCompat.MEASURED_STATE_MASK);
        graph.getLegendRenderer().setTextColor(ViewCompat.MEASURED_STATE_MASK);
        graph.getLegendRenderer().setVisible(true);
        graph.getLegendRenderer().setAlign(LegendRenderer.LegendAlign.TOP);
        graph.getLegendRenderer().setBackgroundColor(0);
    }

    public static void updateGraphAxis(GraphView graph, ArrayList<LineGraphSeries<DataPoint>> arrayList) {
        graph.getViewport().setXAxisBoundsManual(true);
        graph.getViewport().setMinX(graph.getViewport().getMinX(true) - 2.0d);
        graph.getViewport().setMaxX(graph.getViewport().getMaxX(true) + 2.0d);
        graph.getViewport().setYAxisBoundsManual(false);
    }

    public static void addGraphSeries(GraphView graph, ArrayList<LineGraphSeries<DataPoint>> series) {
        graph.removeAllSeries();
        for (int i = 0; i < series.size(); i++) {
            series.get(i).setDrawDataPoints(true);
            series.get(i).setDataPointsRadius(5.0f);
            series.get(i).setColor(SharedDefines.GRAPH_SERIES_COLORS_CFG[i]);
            graph.addSeries(series.get(i));
        }
        updateGraphAxis(graph, series);
    }

    public static void hideGraphSeries(GraphView graph) {
        graph.getGridLabelRenderer().setGridStyle(GridLabelRenderer.GridStyle.NONE);
        graph.getGridLabelRenderer().setHorizontalLabelsVisible(false);
        graph.getGridLabelRenderer().setVerticalLabelsVisible(false);
    }
}
