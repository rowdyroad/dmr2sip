import React, { Component, PropTypes } from 'react'
import { connect } from 'react-redux'
import * as Actions from '../../actions';
import {browserHistory} from 'react-router'


export default (props) => {

  let mapState = (state) => {
    return {  ...props,
              data: state.main.getIn([props.scope,'response','data'])
                   ? state.main.getIn([props.scope,'response']).toJS()
                   : null,
              error: state.main.hasIn([props.scope,'error'])
                      ? state.main.getIn([props.scope, 'error']) : null
              }
  }

  let mapDispatch = (dispatch) => {
    return {
      actions: {
        fetch: (page, append) => {
          dispatch(Actions.Fetch(props.scope, {
                                                  url: props.apiUrl,
                                                  params: { page: page || 1}
                                              },
                                              {
                                                  concat:  append ? 'data' : false,
                                                  pkAttribute:'point_id'
                                              }))
        }
      }
    }
  }

    return connect(mapState, mapDispatch)((props)=>
                        ( <div>
                          {React.Children.map(props.children, (child)=> {
                            return React.cloneElement(child, {...props, ...child.props});
                          })}
                        </div>))
}










