import React from 'react'
import {Link} from 'react-router'
import * as UI from '../../UIKit'

export default (props) => (
    <Link to={"/routes/" + props.route_id} style={{textAlign:'center'}}>
    #{props.route_id}
    	<h1>{props.name}</h1>
    	<UI.Row>
    		<UI.Col5>
                <UI.Box style={{width:100, height:100, overflow:'hidden'}}>
    			{props.source_point ?
                <h5>
                    <div><i className={"fa fa-circle point-status-" + (props.source_point.status ? 1 : 0)}></i></div>
                    {props.source_point.name}
                    <div>
                        <small>{props.source_point.type}</small>
                    </div>
                </h5> : <div style={{paddingTop:10}}><i className="ti-close" style={{color: 'red'}}></i><br/>deleted</div> }
                </UI.Box>
    		</UI.Col5>
            <UI.Col2 style={{paddingTop:40}}><i className="route-direction fa fa-arrow-right"></i></UI.Col2>
    		<UI.Col5>
                <UI.Box style={{width:100, height:100, overflow:'hidden'}}>
    			{props.destination_point ?
                <h5>
                    <div><i className={"fa fa-circle point-status-" + (props.destination_point.status  ? 1: 0)}></i></div>
                    {props.destination_point.name}
                    <div>
                        <small>{props.destination_point.type}</small>
                    </div>
                </h5> : <div style={{paddingTop:10}}><i className="ti-close" style={{color: 'red'}}></i><br/>deleted</div> }
                </UI.Box>
    		</UI.Col5>
    	</UI.Row>
    </Link>
)