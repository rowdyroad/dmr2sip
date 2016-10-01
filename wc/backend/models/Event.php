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
                    [['source_point_id', 'source', 'type'], 'required'],
                    [['route_id', 'result', 'destination'], 'safe']
                ];
    }

    public function getSourcePoint()
    {
        return $this->hasOne(Point::className(), ['point_id'=>'source_point_id']);
    }

    public function getRoute()
    {
	   return $this->hasOne(Route::className(), ['route_id'=>'route_id']);
    }

    public function getDuration()
    {
        return strtotime($this->end_time) - strtotime($this->time);
    }

    public function fields()
    {
	   return array_merge(parent::fields(),['route', 'source_point'=>'sourcePoint', 'duration']);
    }

    public function toArray(array $fields = [],array $expand = [], $recursive = true )
    {
       $arr = parent::toArray($fields, $expand, $recursive);
       $arr['source'] = json_decode($arr['source'], true);
       $arr['destination'] = json_decode($arr['destination'], true);
       return $arr;
    }

}
