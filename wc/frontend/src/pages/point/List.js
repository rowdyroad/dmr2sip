import React, { Component, PropTypes } from 'react'
import { connect } from 'react-redux'
import {Table,TableColumn} from '../../components/Table';
import PageLocation from '../../utils/PageLocation';
import * as Actions from '../../actions';
import {Router, browserHistory, Link} from 'react-router'
import * as UI from '../../components/UIKit'
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
                        <Table className="table-hover" data={this.props.data.data}>
                            <TableColumn name="name" title='Name'>
                                {(props) => (
                                    <Link to={ PageLocation(this.props.routes) + '/' + props.item.point_id }>
                                        <i className={"fa fa-circle point-status-" + props.item.status}></i>&nbsp;{props.item.name}
                                    </Link>
                                )}
                            </TableColumn>
                            <TableColumn name="type" title="Type"/>
                        </Table>
                    </div>
                );
    }
}

let mapState = (state) => {
    return {
                url:"/points",
                pkAttribute:"point_id",
                data: state.main.getIn(['points','success'])
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
                                            params: { page: page || 1},
                                            options: append ? {concat: 'data'} : null
                                         }));
      }
    }
  }
}

export default connect(mapState, mapDispatch)(List)