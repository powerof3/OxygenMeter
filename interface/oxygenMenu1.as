import gfx.io.GameDelegate;
import gfx.managers.FocusHandler;
import gfx.ui.InputDetails;
import gfx.ui.NavigationCode;
import Shared.GlobalFunc;
import Components.Meter;
import skyui.util.Tween;
import mx.utils.Delegate;
import skse;

class oxygenMenu1 extends MovieClip
{
	var amountText: TextField;
	
	function oxygenMenu1()
	{
		super();
		setAmountText(100);
	}
	
	function setAmountText(aNumber: Number): Void
	{
		amountText.text = aNumber.toString();
	}
		
}