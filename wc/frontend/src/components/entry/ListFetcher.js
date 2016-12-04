import React, {Component} from 'react'
import { connect } from 'react-redux'
import * as Actions from '../../actions'
import {fromJS} from 'immutable'

export default (props) => {
  let mapState = (state) => {
    return            { ...props,
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
                                                  pkAttribute: props.pkAttribute
                                              }))
        }
      }
    }
  }


  let mapMerge = (a,b,c) => {
    let state = fromJS(a).mergeDeep(fromJS(b)).toJS();
    return {...c, ...state.map ? {[state.map]: state} : state}
  }

  class ListProxy extends Component {

      componentWillMount = () => {
        if (this.props.autoFetch) {
          this.props.actions.fetch()
        }
      }

      render = () => {
        return <div id={"list-fetcher_" + this.props.scope}>
                  {React.Children.map(this.props.children, (child)=> {
                    return React.cloneElement(child, {...this.props,...child.props});
                  })}
                </div>
      }
  }
  return connect(mapState, mapDispatch, mapMerge)(ListProxy)
}










