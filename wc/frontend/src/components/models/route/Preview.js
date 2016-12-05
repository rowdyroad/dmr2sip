import React from 'react'
import {Link} from 'react-router'
import * as UI from '../../UIKit'

export default (props) => (
    <Link to={"/routes/" + props.route_id} style={{textAlign:'center'}}>
    #{props.route_id}
    	<h1>{props.name}</h1>
    	<UI.Row>
    		<UI.Col5>
    			{props.source_point ? <h3>
                    <i className={"fa fa-circle point-status-" + props.source_point.status}></i>&nbsp;{props.source_point.name}
                    <small style={{ display:'block', fontSize:14, paddingLeft:40}}>{props.source_point.type}</small>
                </h3> : <div style={{height:80}}><i className="ti-close" style={{color: 'red'}}></i><br/>deleted</div> }
    		</UI.Col5>
            <UI.Col2><i className="route-direction fa fa-arrow-right"></i></UI.Col2>
    		<UI.Col5>
    			{props.destination_point ? <h3>
                    <i className={"fa fa-circle point-status-" + props.destination_point.status}></i>&nbsp;{props.destination_point.name}
                    <small style={{ display:'block', fontSize:14, paddingLeft:40}}>{props.destination_point.type}</small>
                </h3> : <div style={{height:80}}><i className="ti-close" style={{color: 'red'}}></i><br/>deleted</div> }
    		</UI.Col5>
    	</UI.Row>
    </Link>
)