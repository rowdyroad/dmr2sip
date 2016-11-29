import React, { Component, PropTypes } from 'react'
import { connect } from 'react-redux'
import {Router, browserHistory, Link} from 'react-router'
import ItemLoader from '../../components/ItemLoader'
import * as UI from '../../components/UIKit'
import * as Actions from '../../actions';

const SIPView = (props) => (
    <div>
        <UI.Row>
            <UI.Col2 style={{padding:'1em'}}>
                <UI.Property label="SIP Host">{props.host}</UI.Property>
            </UI.Col2>
            <UI.Col2 style={{padding:'1em'}}>
                <UI.Property label="SIP Port">{props.port}</UI.Property>
            </UI.Col2>
        </UI.Row>
        <UI.Row>
            <UI.Col2 style={{padding:'1em'}}>
                <UI.Property label="Username">{props.authorization_username}</UI.Property>
            </UI.Col2>
            <UI.Col2 style={{padding:'1em'}}>
                <UI.Property label="Password">********</UI.Property>
            </UI.Col2>
        </UI.Row>
        <UI.Row>
            <UI.Col2 style={{padding:'1em'}}>
                <UI.Property label="Caller ID">{props.username}</UI.Property>
            </UI.Col2>
        </UI.Row>
    </div>
)
const DMRView = (props) => (
    <UI.Area>
        <UI.Row>
            <UI.Col4 style={{padding:'1em'}}>
                <UI.Property label="Sound Device Index">{props.device_index}</UI.Property>
            </UI.Col4>
        </UI.Row>
        <UI.Row>
            <UI.Col2 style={{padding:'1em'}}>
                <UI.Property label="Device Host">{props.address}</UI.Property>
            </UI.Col2>
            <UI.Col2 style={{padding:'1em'}}>
                <UI.Property label="Device Port">{props.port}</UI.Property>
            </UI.Col2>
        </UI.Row>
        <h6 className="header-title">Phone mode interruption</h6>
        <UI.Row>
            <UI.Col4 style={{padding:'1em'}}>
                <UI.Property label="Code">{props.phone_mode.code}</UI.Property>
            </UI.Col4>
            <UI.Col4 style={{padding:'1em'}}>
                <UI.Property label="Duration">{props.phone_mode.duration} ms</UI.Property>
            </UI.Col4>
        </UI.Row>
    </UI.Area>
)

const PointView = (props) => (
    <UI.Area>
        <UI.Row>
            <UI.Col10>
                <h1>
                    <i className={"fa fa-circle point-status-" + (props.status ? props.status : 0)} ></i>&nbsp;{props.name}
                    <small style={{ display:'block', fontSize:14, paddingLeft:40}}>{props.type}</small>
                </h1>
            </UI.Col10>
            <UI.Col2 style={{textAlign:'right'}}>
                <UI.Button to={'/points/' + props.point_id + '/edit'} icon="ti-pencil"/>
                &nbsp;
                {props.onRemove ? <UI.Button onClick={props.onRemove} color="danger" icon="ti-trash"/> : null}
            </UI.Col2>
        </UI.Row>
        {props.type === 'dmr' ? <DMRView phone_mode={{}} {...props.configuration}/> : null}
        {props.type === 'sip' ? <SIPView {...props.configuration || {}}/> : null}
    </UI.Area>
)

class PointViewContainer extends Component
{
    render = () => {
        if (!this.props.data) {
            return null;
        }
        return (
            <UI.Box>
                <PointView {...this.props.data} onRemove={this.props.actions.showRemoveModal}/>
            </UI.Box>
        )
    }
}

export default PointViewContainer;