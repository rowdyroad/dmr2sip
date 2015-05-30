<?php

namespace app\models;

class Event extends \yii\db\ActiveRecord
{
	static public function tableName()
	{
		return 'events';
	}

 	public function rules()
    {
    	return 	[
    				[['source_point','destination_point'], 'required']
    			];
    }
}
