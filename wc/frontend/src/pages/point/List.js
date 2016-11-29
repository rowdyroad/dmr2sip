import React, { Component, PropTypes } from 'react'
import { connect } from 'react-redux'
import {Table,TableColumn} from '../../components/Table';
import PageLocation from '../../utils/PageLocation';
import * as Actions from '../../actions';
import {Router, browserHistory, Link} from 'react-router'
import * as UI from '../../components/UIKit'

const Item = (props) => (
    <Link to={"/points/" + props.point_id} style={{textAlign:'center'}}>
    {props.point_id}
        <div className={"point-item-status point-item-status-" + (props.status ? props.status : 0)}></div>
        <h1 className="point-item-name">{props.name}</h1>
        <h4 className="point-item-type">{props.type}</h4>
        {props.type === 'dmr' && props.configuration ? <h6 className="point-item-configuration">Sound Device: {props.configuration.device_index}</h6> : null}
        {props.type === 'sip' && props.configuration ? <h6 className="point-item-configuration">Caller ID: {props.configuration.username}({props.configuration.host}:{props.configuration.port})</h6> : null}
   </Link>

)
class List extends Component
{
    componentWillMount = () => {
        if (!this.props.data) {
            this.props.actions.fetch()
        }
    }

    render = () => {
        if (!this.props.data) {
            return null;
        }
        return (
                    <div>
                        <UI.Button  to={this.props.url + "/new"}
                                    label="Add"
                                    icon="glyphicon glyphicon-plus-sign"
                                    style={{marginBottom:'1em',right:0}}/>
                        <UI.Row>
                        {this.props.data.data.map((item) => (
                            <UI.Col3 key={item.point_id}>
                                <UI.Box>
                                    <Item {...item}/>
                                </UI.Box>
                            </UI.Col3>
                        ))}
                        </UI.Row>
                        {this.props.data._meta.currentPage < this.props.data._meta.pageCount ?
                            <UI.Button label="More" type="button" onClick={()=>{this.props.actions.fetch(this.props.data._meta.currentPage + 1,true)}}/>
                            : null }
                    </div>
                );
    }
}

let mapState = (state) => {
    return {
                url:"/points",
                pkAttribute:"point_id",
                data: state.main.getIn(['points','response','data'])
                     ? state.main.getIn(['points','response']).toJS()
                     : null,
                error: state.main.hasIn(['points','error'])
                    ? state.main.getIn(['points', 'error']) : null
            }
}

let mapDispatch = (dispatch) => {
  return {
    actions: {
      fetch: (page, append) => {
        dispatch(Actions.Fetch("points", {  url: "/api/points",
                                            params: { page: page || 1}
                                         }, {concat:  append ? 'data' : false }))
      }
    }
  }
}

export default connect(mapState, mapDispatch)(List)