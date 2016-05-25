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
        return  [
                    [['route_id','source_number','type'], 'required']
                ];
    }


    public function getRoute()
    {
	   return $this->hasOne(Route::className(), ['route_id'=>'route_id']);
    }

    public function fields()
    {
	   return array_merge(parent::fields(),['route']);
    }

    public function toArray( $fields = [], $expand = [], $recursive = true )
    {
       $arr = parent::toArray($fields, $expand, $recursive);
       $arr['source_number'] = json_decode($arr['source_number'],true);
       return $arr;
    }

}
