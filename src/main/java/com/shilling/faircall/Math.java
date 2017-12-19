package com.shilling.faircall;

import java.util.Collection;
import java.util.List;
import java.util.stream.Collectors;

import com.google.common.base.Preconditions;
import com.google.inject.Singleton;

@Singleton
public class Math {

	public double median (Collection<Integer> in) {
		Preconditions.checkNotNull(in);
		
		List<Integer> data = in.stream()
				.sorted(Integer::compare)
				.collect(Collectors.toList());
		
		if (data.size() % 2 == 0) {
			double x = data.get(data.size() / 2);
			double y = data.get((data.size() / 2) - 1);
			return (x + y) / 2.0;
		} else {
			return data.get(data.size() / 2);
		}
	}
	
	public double lowQ (Collection<Integer> in) {
		Preconditions.checkNotNull(in);
		
		List<Integer> data = in.stream()
				.sorted(Integer::compare)
				.collect(Collectors.toList());
		
		if (data.size() % 2 == 0) {
			return this.median(data.subList(0, (data.size() / 2) - 1));
		} else {
			return this.median(data.subList(0, data.size() / 2));
		}
	}
	
	public double upperQ (Collection<Integer> in) {
		Preconditions.checkNotNull(in);
		
		List<Integer> data = in.stream()
				.sorted(Integer::compare)
				.collect(Collectors.toList());
		
		if (data.size() % 2 == 0) {
			return this.median(data.subList((data.size() / 2), data.size()));
		} else {
			return this.median(data.subList((data.size() / 2) + 1, data.size()));
		}
	}
	
	public double iqr (Collection<Integer> in) {
		Preconditions.checkNotNull(in);
		return this.upperQ(in) - this.lowQ(in);
	}
	
	public double max (Collection<Integer> in) {
		return this.upperQ(in) + ( 1.5 * this.iqr(in) );
	}
	
	public double min (Collection<Integer> in) {
		return this.lowQ(in) - ( 1.5 * this.iqr(in) );
	}
}
