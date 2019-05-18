//*****************************************************************************
// Copyright 2017-2019 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//*****************************************************************************

#pragma once

#include <future>
#include <memory>

#include "ngraph/function.hpp"
#include "ngraph/runtime/performance_counter.hpp"
#include "ngraph/shape.hpp"
#include "ngraph/type/element_type.hpp"

namespace ngraph
{
    namespace runtime
    {
        class Tensor;
        class Executable;
    }
}

class ngraph::runtime::Executable
{
public:
    Executable();
    virtual ~Executable();

    /// \param outputs vector of runtime::Tensor used as outputs
    /// \param inputs vector of runtime::Tensor used as inputs
    /// \returns true if iteration is successful, false otherwise
    virtual bool call(const std::vector<std::shared_ptr<runtime::Tensor>>& outputs,
                      const std::vector<std::shared_ptr<runtime::Tensor>>& inputs) = 0;

    /// \brief Executes a single iteration of a Function.
    /// \param outputs vector of runtime::Tensor used as outputs
    /// \param inputs vector of runtime::Tensor used as inputs
    /// \returns true if iteration is successful, false otherwise
    bool call_with_validate(const std::vector<std::shared_ptr<runtime::Tensor>>& outputs,
                            const std::vector<std::shared_ptr<runtime::Tensor>>& inputs);

    /// \brief Asynchronously executes a single iteration of the Function. The `future` is used
    ///     to monitor the execution.
    /// \param outputs vector of runtime::Tensor used as outputs
    /// \param inputs vector of runtime::Tensor used as inputs
    /// \returns a valid std::future to monitor the execution. Use future.get() to get the results
    ///     or future.wait*() to wait for completion.
    virtual std::future<bool>
        begin_execute(const std::vector<std::shared_ptr<runtime::Tensor>>& outputs,
                      const std::vector<std::shared_ptr<runtime::Tensor>>& inputs);

    /// \brief Collect performance information gathered on a Function.
    /// \returns Vector of PerformanceCounter information.
    virtual std::vector<PerformanceCounter> get_performance_data() const;

    /// \brief Validates a Function.
    /// \param outputs vector of runtime::Tensor used as outputs
    /// \param inputs vector of runtime::Tensor used as inputs
    void validate(const std::vector<std::shared_ptr<runtime::Tensor>>& outputs,
                  const std::vector<std::shared_ptr<runtime::Tensor>>& inputs);

    /// \brief Query the input Parameters
    /// \returns an ngraph::op::ParameterVector of all input parameters
    const ngraph::ParameterVector& get_parameters() const;

    /// \brief Query the output Results
    /// \returns an ngraph::ResultVector of all input parameters
    const ngraph::ResultVector& get_results() const;

    /// \brief Save this compiled Executable to an output stream.
    ///    Saved stream may be read with Backend::load
    virtual void save(std::ostream& output_stream);

protected:
    /// \brief Called at the end of compile to the values to be returned by get_parameters
    ///     and get_results
    /// \param func The function with Results fully resolved.
    void set_parameters_and_results(const Function& func);

    bool begin_execute_helper(const std::vector<std::shared_ptr<runtime::Tensor>>& outputs,
                              const std::vector<std::shared_ptr<runtime::Tensor>>& inputs);

    class ReadWriteInfo
    {
    public:
        ReadWriteInfo(void* p, size_t size, bool is_read)
            : m_data{p}
            , m_size_in_bytes{size}
            , m_is_read{is_read}
        {
        }
        bool is_read() const { return m_is_read; }
        bool is_write() const { return !is_read(); }
        void* get_ptr() const { return m_data; }
        bool get_size_in_bytes() const { return m_size_in_bytes; }
    private:
        void* m_data;
        size_t m_size_in_bytes;
        bool m_is_read;
    };

private:
    ngraph::ParameterVector m_parameters;
    ngraph::ResultVector m_results;
};
