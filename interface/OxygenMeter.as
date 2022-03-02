import gfx.io.GameDelegate;
import gfx.managers.FocusHandler;
import gfx.ui.InputDetails;
import gfx.ui.NavigationCode;
import Shared.GlobalFunc;
import Components.Meter;
import skyui.util.Tween;
import mx.utils.Delegate;
import com.greensock.TimelineLite;
import com.greensock.easing.*;

class OxygenMeter extends MovieClip
{
	var MeterContainer:MovieClip;
	var meterDuration:Number;
	var minWidth:Number;
	var maxWidth:Number;
	var Percent:Number;

	var targetPercent:Number;

	var MeterTimeline:TimelineLite;

	function OxygenMeter()
	{
		super();
		targetPercent = 0;
		meterDuration = 0.01;

		MeterTimeline = new TimelineLite({paused:true});
		maxWidth = MeterContainer.Mask._x;
		minWidth = MeterContainer.Mask._x - MeterContainer.Mask._width;
		Percent = (maxWidth - minWidth) / 100;
	}
	
	function onLoad(): Void
	{
		this.gotoAndStop("hide");
		//doFadeOut();
		//doShow();
	}
	
	public function setLocation(xpos: Number, ypos: Number, rot: Number): Void
	{
		this._x = xpos;
		this._y = ypos;
		this._rotation = rot;
	}
	
	public function doFadeOut(): Void
	{
		if (this._currentframe == 1)
		{
			this.gotoAndPlay("fadeout");
		}
	}
		
	public function doShow(): Void
	{
		if (this._currentframe != 1)
		{
			this.gotoAndStop("show");
		}
	}
	
	public function setMeterPercent(CurrentPercent:Number):Void
	{
		MeterTimeline.clear();
		CurrentPercent = doValueClamp(CurrentPercent);
		MeterContainer.Mask._x = minWidth + (Percent * CurrentPercent);
	}

	public function doValueClamp(clampValue:Number):Number
	{
		return clampValue > 100 ? 100 : (clampValue <= 0 ? -1 : clampValue);
	}

	public function updateMeterPercent(CurrentPercent:Number):Void
	{
		CurrentPercent = doValueClamp(CurrentPercent);

		if (!MeterTimeline.isActive())
		{
			MeterTimeline.clear();
			MeterTimeline.progress(0);
			MeterTimeline.restart();
		}
		MeterTimeline.to(MeterContainer.Mask,1,{_x:minWidth +(Percent * CurrentPercent)},MeterTimeline.time() + meterDuration);
		MeterTimeline.play();
	}

}