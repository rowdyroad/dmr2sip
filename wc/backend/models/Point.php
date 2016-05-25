<?php

namespace app\models;

class Point extends \yii\db\ActiveRecord
{
    static public function tableName()
    {
        return 'points';
    }

    public function rules()
    {
        return  [
                    [['type','name'], 'required'],
		            ['configuration', 'filter', 'filter'=>'json_encode']
                ];
    }

    public function toArray( $fields = [], $expand = [], $recursive = true )
    {
	   $arr = parent::toArray($fields, $expand, $recursive);
	   $arr['configuration'] = json_decode($arr['configuration'],true);
	   return $arr;
    }
}
