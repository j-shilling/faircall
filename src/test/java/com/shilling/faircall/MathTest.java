package com.shilling.faircall;

import java.util.Arrays;
import java.util.List;

import junit.framework.TestCase;

public class MathTest extends TestCase {
	
	private final Math math = new Math();
	private final List<Integer> data =
			Arrays.asList(new Integer[] {71, 70, 73, 70, 70, 69, 70, 72, 71, 300, 71, 69 });

	public void testMedian() {
		assertEquals(70.5, this.math.median(this.data));
	}
	
	public void testLowQ () {
		assertEquals (70.0, this.math.lowQ(this.data));
	}
	
	public void testUpperQ () {
		assertEquals (71.5, this.math.upperQ(this.data));
	}
	
	public void testIqr () {
		assertEquals (1.5, this.math.iqr(this.data));
	}
	
	public void testMax () {
		assertEquals (73.75, this.math.max(this.data));
	}
	
	public void testMin () {
		assertEquals (67.75, this.math.min(this.data));
	}
	
	public void testStandardDeviation () {
		assertEquals (63.427561482021005, this.math.standardDeviation(this.data));
	}

}
